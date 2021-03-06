# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
Jadie Adams

The femur data set is comprised of segmented meshes of femurs and corresponding CT images that are not segmented.
The first step in grooming is to turn the meshes into the binary volume format shapeworks expects.
The full mages and segmentations must be carried through every stop of grooming. 
Optimization uses single scale.


First import the necessary modules
"""

from zipfile import ZipFile
import os
import sys
import csv
import argparse
import glob
import re
import numpy as np

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *

from DatasetUtils import datasets

def Run_Pipeline(args):

    """
    Unzip the data for this tutorial.

    The data is inside the leftatrium.zip, run the following function to unzip the
    data and create necessary supporting files. The files will be Extracted in a
    newly created Directory TestEllipsoids.
    This data is LGE segmentation of left atrium.
    """
    """
    Extract the zipfile into proper directory and create necessary supporting
    files
    """
    print("\nStep 1. Get Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    filename = "femurdata.zip"
    parentDir="TestFibula/"
    inputDir = 'TestFemur/femurdata/'

    if not os.path.exists(parentDir):
        os.makedirs(parentDir)

    # # Check if the data is in the right place
    if not os.path.exists(filename):
        print("Can't find " + filename + " on the local filesystem.")
        print("Downloading " + filename + " from SCIGirder.")
        datasets.downloadDataset(filename)

    # extract the zipfile
    # print("Extracting data from " + filename + "...")
    # with ZipFile(filename, 'r') as zipObj:
    #     zipObj.extractall(path=parentDir)

    print("\nStep 2. Groom - Data Pre-processing\n")
    if args.interactive:
        input("Press Enter to continue")

    if not args.start_with_prepped_data:
        """
        ## GROOM : Data Pre-processing
        For the unprepped data the first few steps are
        -- if no interactive tag - define cutting plane
        -- Reflect images and meshes
        -- Turn meshes to volumes
        -- Isotropic resampling
        -- Padding
        -- Center of Mass Alignment
        -- Rigid Alignment
        -- if interactive tag - define cutting plane
        -- clip segementations with cutting plane
        -- Largest Bounding Box and Cropping
        """

        # Directory where grooming output folders will be added
        parentDir = 'TestFemur/PrepOutput/'
        if not os.path.exists(parentDir):
            os.mkdir(parentDir)
                # set name specific variables
        img_suffix = "1x_hip"
        reference_side = "left" # somewhat arbitrary

        # Get image ane mesh segmentation file lists
        files_img = []
        img_dir = inputDir + 'images/'
        for file in sorted(os.listdir(img_dir)):
            files_img.append(img_dir + file)
        files_mesh = []
        mesh_dir = inputDir + 'meshes/'
        for file in sorted(os.listdir(mesh_dir)):
            files_mesh.append(mesh_dir + file)

        if args.tiny_test:
            files_img = files_img[:3]
            files_mesh = files_mesh[:3]

        if args.use_subsample:
            sample_idx = sampledata(files_img, int(args.use_subsample))
            files_img = [files_img[i] for i in sample_idx]
            files_mesh = [files_mesh[i] for i in sample_idx]

        # If not interactive, get cutting plane on a mesh user specifies
        if not args.interactive:
            options = []
            for file in files_mesh:
                file = file.split('/')[-1]
                prefix = "_".join(file.split("_")[:2])
                options.append(prefix)
            input_mesh = ''
            while not input_mesh:
                print("\n\nType the prefix of the sample you wish to use to select the cutting plane from listed options and press enter.\nOptions: " + ", ".join(options) + '\n')
                cp_prefix = input()
                if cp_prefix:
                    for file in files_mesh:
                        if cp_prefix in file:
                            input_mesh = file
                if not input_mesh:
                    print("Invalid prefix.")
            cutting_plane_points = SelectCuttingPlane(input_mesh)
            if cp_prefix[-1] =='R':
                reference_side = "right"
            print("Cutting plane points defined: ")
            print(cutting_plane_points)
            print("Continuing to groom.")

        """
        Reflect - We have left and right femurs, so we reflect both image and mesh 
        for the non-reference side so that all of the femurs can be aligned.
        """
        reflectedFiles_mesh, reflectedFile_img = anatomyPairsToSingles(parentDir + 'reflected', files_mesh, files_img, reference_side)

        """
        MeshesToVolumes - Shapeworks requires volumes so we need to convert 
        mesh segementaions to binary segmentations.
        """
        fileList_seg = MeshesToVolumes(parentDir + "volumes", reflectedFiles_mesh, reflectedFile_img)

        """
        Apply isotropic resampling - The segmentation and images are resampled independently to have uniform spacing.
        """
        resampledFiles_segmentations = applyIsotropicResampling(parentDir + "resampled/segmentations", fileList_seg, recenter=False, isBinary=True)
        resampledFiles_images = applyIsotropicResampling(parentDir + "resampled/images", reflectedFile_img, recenter=False, isBinary=False)
        """
        Apply padding - Both the segmentation and raw images are padded in case the seg lies on the image boundary.
        """
        paddedFiles_segmentations = applyPadding(parentDir + "padded/segementations/", resampledFiles_segmentations, 10)
        paddedFiles_images = applyPadding(parentDir + "padded/images/", resampledFiles_images, 10)

        """
        Apply center of mass alignment - This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
        If raw=the list of image files, it will process both. If the raw parameter is left out it will process the segmentations only.
        """
        [comFiles_segmentations, comFiles_images] = applyCOMAlignment( parentDir + "com_aligned", paddedFiles_segmentations, raw=paddedFiles_images)

        centerFiles_segmentations = center(parentDir + "centered/segmentations/", comFiles_segmentations)
        centerFiles_images = center(parentDir + "centered/images/", comFiles_images)

        """
        Apply rigid alignment
        This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
        There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
        processRaw = False, applies the center of mass alignment only on segemnattion data.
        This function uses the same transfrmation matrix for alignment of raw and segmentation files.
        Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
        """
        medianFile = FindReferenceImage(centerFiles_segmentations)

        [rigidFiles_segmentations, rigidFiles_images] = applyRigidAlignment(parentDir, centerFiles_segmentations, centerFiles_images , medianFile, processRaw = True)

        # Define cutting plane on median sample
        if args.interactive:
           input_file = medianFile.replace("centered","aligned").replace(".nrrd", ".aligned.DT.nrrd")
           cutting_plane_points = SelectCuttingPlane(input_file)
        # Fix cutting plane points previously selected
        else:
            # Get COM translation
            COM_folder = parentDir + "com_aligned/segmentations/"
            for file in os.listdir(COM_folder):
                if cp_prefix in file and ".txt" in file:
                    COM_filename = COM_folder + file
            COM_filehandler = open(COM_filename, "r")
            line = COM_filehandler.readlines()[2].replace("translation:","")
            trans = []
            for string in line.split():
                trans.append(float(string))
            trans = np.array(trans)
            COM_filehandler.close()
            # Apply COM translation
            print("Translating cutting plane by: ")
            print(trans)
            new_cutting_plane_points = np.zeros(cutting_plane_points.shape)
            for pt_index in range(cutting_plane_points.shape[0]):
                new_cutting_plane_points[pt_index] = cutting_plane_points[pt_index] - trans
            cutting_plane_points = new_cutting_plane_points
            # Get center translation
            center_folder = parentDir + "centered/segmentations/"
            for file in os.listdir(center_folder):
                if cp_prefix in file and ".txt" in file:
                    center_filename = center_folder + file
            center_filehandler = open(center_filename, "r")
            line = center_filehandler.readlines()[0]
            center_trans = np.array(line.split())
            center_trans= center_trans.astype(float)
            # Apply center translation
            print("Translating cutting plane by: ")
            print(center_trans)
            new_cutting_plane_points = np.zeros(cutting_plane_points.shape)
            for pt_index in range(cutting_plane_points.shape[0]):
                new_cutting_plane_points[pt_index] = cutting_plane_points[pt_index] - center_trans
            cutting_plane_points = new_cutting_plane_points
            # Get rigid transformation
            rigid_folder = parentDir + "aligned/transformations/"
            for file in os.listdir(rigid_folder):
                if cp_prefix in file and img_suffix not in file:
                    rigid_filename = rigid_folder + file
            rigid_filehandler = open(rigid_filename, "r")
            matrix = []
            lines = rigid_filehandler.readlines()
            index = 0
            for line in lines:
                matrix.append([])
                for string in line.split():
                    matrix[index].append(float(string))
                index += 1
            matrix = np.array(matrix)
            rigid_filehandler.close()
            print("Transforming cutting plane by: ")
            print(matrix)
            new_cutting_plane_points = np.zeros(cutting_plane_points.shape)
            for pt_index in range(cutting_plane_points.shape[0]):
                pt4D = np.array([1,1,1,1])
                pt4D[:3] = cutting_plane_points[pt_index]
                pt = matrix.dot(pt4D)
                new_cutting_plane_points[pt_index] = pt
            cutting_plane_points = new_cutting_plane_points
            # catch for flipped norm
            if cutting_plane_points[0][1] < 0 and cutting_plane_points[1][1] < 0 and cutting_plane_points[2][1] < 0 :
                cutting_plane_points[0][1] = cutting_plane_points[0][1] *-1
                cutting_plane_points[1][1] = cutting_plane_points[1][1] *-1
                cutting_plane_points[2][1] = cutting_plane_points[2][1] *-1
            print("Cutting plane points: ")
            print(cutting_plane_points)

        """
        Clip Binary Volumes - We have femurs of different shaft length so we will clip them all using the defined cutting plane.
        """
        clippedFiles_segmentations = ClipBinaryVolumes(parentDir + 'clipped_segmentations', rigidFiles_segmentations, cutting_plane_points.flatten())

        """
        Compute largest bounding box and apply cropping - 
        """
        [croppedFiles_segmentations, croppedFiles_images] = applyCropping(parentDir, clippedFiles_segmentations,  rigidFiles_images, processRaw=True)


        print("\nStep 3. Groom - Convert to distance transforms\n")
        if args.interactive:
            input("Press Enter to continue")

        """
        We convert the scans to distance transforms, this step is common for both the
        prepped as well as unprepped data, just provide correct filenames.
        """

        dtFiles = applyDistanceTransforms(parentDir, croppedFiles_segmentations)

    else:
        # dtFiles = applyDistanceTransforms(parentDir, fileList_seg)

        dtFiles = []
        dt_dir = parentDir + 'PrepOutput/'

        for file in sorted(os.listdir(dt_dir)):
            dtFiles.append(dt_dir + file)

        print("\nStep 3. Groom - Convert to distance transforms\n")
        if args.interactive:
            input("Press Enter to continue")
    
    """

    ## OPTIMIZE : Particle Based Optimization

    Now that we have the distance transform representation of data we create
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer to
    'https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/ParameterDescription.pdf'

    We provide two different mode of operations for the ShapeWorks particle opimization;
    1- Single Scale model takes fixed number of particles and performs the optimization.
    For more detail about the optimization steps and parameters please refer to
    'https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/ScriptUsage.pdf'

    2- Multi scale model optimizes for different number of particles in hierarchical manner.
    For more detail about the optimization steps and parameters please refer to
    'https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/ScriptUsage.pdf'

    First we need to create a dictionary for all the parameters required by these
    optimization routines
    """
    print("\nStep 4. Optimize - Particle Based Optimization\n")
    if args.interactive:
        input("Press Enter to continue")

    pointDir = parentDir + 'PointFiles/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    if args.use_single_scale:
        parameterDictionary = {
            "number_of_particles" : 1024, 
            "use_normals": 0,
            "normal_weight": 10.0,
            "checkpointing_interval" : 10,
            "keep_checkpoints" : 1,
            "iterations_per_split" : 4000,
            "optimization_iterations" : 4000,
            "starting_regularization" : 100,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 10,
            "initial_relative_weighting" : 1,
            "procrustes_interval" : 1,
            "procrustes_scaling" : 1,
            "save_init_splits" : 1,
            "debug_projection" : 0,
            "verbosity" : 3,
            "use_statistics_in_init" : 0
        }

        """
        Now we execute the particle optimization function.
        """
        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_SingleScale(pointDir, dtFiles, parameterDictionary)

    else:
        parameterDictionary = {
            "starting_particles" : 64,
            "number_of_levels" : 5,
            "use_normals": 0, 
            "normal_weight": 10.0,
            "checkpointing_interval" : 10,
            "keep_checkpoints" : 1,
            "iterations_per_split" : 4000,
            "optimization_iterations" : 4000,
            "starting_regularization" : 100,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 10,
            "initial_relative_weighting" : 1,
            "procrustes_interval" : 1, 
            "procrustes_scaling" : 1,
            "save_init_splits" : 1, 
            "debug_projection" : 0,
            "verbosity" : 3,
            "use_statistics_in_init" : 0
        }


        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_MultiScale(pointDir, dtFiles, parameterDictionary)



    """
    ## ANALYZE : Shape Analysis and Visualization

    Shapeworks yields relatively sparse correspondence models that may be inadequate to reconstruct
    thin structures and high curvature regions of the underlying anatomical surfaces.
    However, for many applications, we require a denser correspondence model, for example,
    to construct better surface meshes, make more detailed measurements, or conduct biomechanical
    or other simulations on mesh surfaces. One option for denser modeling is
    to increase the number of particles per shape sample. However, this approach necessarily
    increases the computational overhead, especially when modeling large clinical cohorts.

    Here we adopt a template-deformation approach to establish an inter-sample dense surface correspondence,
    given a sparse set of optimized particles. To avoid introducing bias due to the template choice, we developed
    an unbiased framework for template mesh construction. The dense template mesh is then constructed
    by triangulating the isosurface of the mean distance transform. This unbiased strategy will preserve
    the topology of the desired anatomy  by taking into account the shape population of interest.
    In order to recover a sample-specific surface mesh, a warping function is constructed using the
    sample-level particle system and the mean/template particle system as control points.
    This warping function is then used to deform the template dense mesh to the sample space.

    """


    """
    Reconstruct the dense mean surface given the sparse correspondence model.
    """

    print("\nStep 5. Analysis - Reconstruct the dense mean surface given the sparse correspodence model.\n")
    if args.interactive:
        input("Press Enter to continue")
    launchShapeWorksStudio(pointDir, dtFiles, localPointFiles, worldPointFiles)

