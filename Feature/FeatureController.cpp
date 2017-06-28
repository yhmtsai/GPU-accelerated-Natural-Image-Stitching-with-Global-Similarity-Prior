//
//  FeatureController.cpp
//  UglyMan_Stitching
//
//  Created by uglyman.nothinglo on 2015/8/15.
//  Copyright (c) 2015 nothinglo. All rights reserved.
//

#include "FeatureController.h"
#include <cublas_v2.h>
#include <cuda_runtime.h>

void FeatureDescriptor::addDescriptor(const Mat & _descriptor) {
    data.emplace_back(_descriptor);
}

double FeatureDescriptor::getDistance(const FeatureDescriptor & _descriptor1,
                                      const FeatureDescriptor & _descriptor2,
                                      const double _threshold) {
    const vector<Mat> & data1 = _descriptor1.data;
    const vector<Mat> & data2 = _descriptor2.data;
    double result = MAXFLOAT;
    // cout<<"Start\n";
    // cin.get();

    // int num_streams=16;
    // cublasHandle_t cublas_handle[num_streams];
    // cudaStream_t stream[num_streams];
    // for (int i=0; i<num_streams; i++){
    //     cublasCreate(&(cublas_handle[i]));
    //     cudaStreamCreateWithFlags(&stream[i],cudaStreamNonBlocking);
    //     cublasSetStream(cublas_handle[i],stream[i]);
    // }
    // float *gpu_data1=NULL, *gpu_data2=NULL;
    // cudaMalloc((void**) &gpu_data1, SIFT_DESCRIPTOR_DIM*data1.size()*sizeof(float));
    // cudaMalloc((void**) &gpu_data2, SIFT_DESCRIPTOR_DIM*data2.size()*sizeof(float));
    // // float **temp_ptr1, **temp_ptr2;
    // // temp_ptr1= new *float [data1.size()];
    // // temp_ptr2= new *float [data2.size()];
    // float *norm1, *norm2;
    // norm1= new float [data1.size()];
    // norm2= new float [data2.size()];
    // for (int i=0; i<data1.size(); i++){
    //     // temp_ptr1[i]= data1[i].ptr<vl_sift_pix>(0);
    //     // cudaHostRegister(data1[i].ptr<vl_sift_pix>(0), SIFT_DESCRIPTOR_DIM* sizeof(float), cudaHostRegisterDefault);
    //     cudaMemcpyAsync(gpu_data1+i*SIFT_DESCRIPTOR_DIM, data1[i].ptr<vl_sift_pix>(0), SIFT_DESCRIPTOR_DIM*sizeof(float), 
    //                     cudaMemcpyHostToDevice, stream[i%num_streams]);
    //     cublasSnrm2(cublas_handle[i%num_streams], SIFT_DESCRIPTOR_DIM, gpu_data1+i*SIFT_DESCRIPTOR_DIM, 1, norm1+i);
    // }
                
    // for (int i=0; i<data2.size(); i++){
    //     // temp_ptr2[i]= data2[i].ptr<vl_sift_pix>(0);
    //     // cudaHostRegister(data2[i].ptr<vl_sift_pix>(0), SIFT_DESCRIPTOR_DIM* sizeof(float), cudaHostRegisterDefault);
    //     cudaMemcpyAsync(gpu_data2+i*SIFT_DESCRIPTOR_DIM, data2[i].ptr<vl_sift_pix>(0), SIFT_DESCRIPTOR_DIM*sizeof(float), 
    //                     cudaMemcpyHostToDevice, stream[i%num_streams]);
    //     cublasSnrm2(cublas_handle[i%num_streams], SIFT_DESCRIPTOR_DIM, gpu_data2+i*SIFT_DESCRIPTOR_DIM, 1, norm2+i);
    // }
    // float *AB;
    // cudaMalloc((void**)&AB, data1.size()* data2.size()*sizeof(float));
    // // std::cout<<data1.size()<<" "<<data2.size()<<" "<<SIFT_DESCRIPTOR_DIM<<"\n";
    // float alpha=-2, zero=0;
    // cublasSgemm(cublas_handle[0], CUBLAS_OP_T, CUBLAS_OP_N, data1.size(), data2.size(), SIFT_DESCRIPTOR_DIM, 
    //             &alpha, gpu_data1, SIFT_DESCRIPTOR_DIM, gpu_data2, SIFT_DESCRIPTOR_DIM, 
    //             &zero, AB, data1.size());
    // cout << data1.size()<<" "<<data2.size()<<"\n";
    for(int i = 0; i < data1.size(); ++i) {
        for(int j = 0; j < data2.size(); ++j) {
            double distance = 0;
            for(int k = 0; k < SIFT_DESCRIPTOR_DIM; ++k) {
                distance += ((data1[i].at<vl_sift_pix>(k) - data2[j].at<vl_sift_pix>(k)) *
                             (data1[i].at<vl_sift_pix>(k) - data2[j].at<vl_sift_pix>(k)));
    /* at<vl_sift_pix>(k) == at<vl_sift_pix>(0, k) */
                
                if(distance >= _threshold) {
                    break;
                }
            }
            // cout<<"d"<<distance<<" ";
            result = min(result, distance);
        }
    }
    // cudaFree(gpu_data1);
    // cudaFree(gpu_data2);
    // cudaFree(AB);
    // delete [] norm1;
    // delete [] norm2;
    // for (int i=0; i<num_streams; i++){
    //     cudaStreamDestroy(stream[i]);
    //     cublasDestroy(cublas_handle[i]);
    // }
    // cout<<"Free\n";
    // cin.get();
    return result;
}

void FeatureController::detect(const Mat & _grey_img,
                               vector<Point2> & _feature_points,
                               vector<FeatureDescriptor> & _feature_descriptors) {
#ifndef NDEBUG
    if(_feature_points.empty() == false) {
        _feature_points.clear();
        printError("F(detect) feature points is not empty");
    }
    if(_feature_descriptors.empty() == false) {
        _feature_descriptors.clear();
        printError("F(detect) feature descriptors is not empty");
    }
#endif
    Mat grey_img_float = _grey_img.clone();
    grey_img_float.convertTo(grey_img_float, CV_32FC1);

    const int  width = _grey_img.cols;
    const int height = _grey_img.rows;
    
    VlSiftFilt * vlSift = vl_sift_new(width, height,
                                      log2(min(width, height)),
                                      SIFT_LEVEL_COUNT,
                                      SIFT_MINIMUM_OCTAVE_INDEX);
    vl_sift_set_peak_thresh(vlSift, SIFT_PEAK_THRESH);
    vl_sift_set_edge_thresh(vlSift, SIFT_EDGE_THRESH);
    
    if(vl_sift_process_first_octave(vlSift, (vl_sift_pix const *) grey_img_float.data) != VL_ERR_EOF) {
        do {
            vl_sift_detect(vlSift);
            for(int i = 0; i < vlSift->nkeys; ++i) {
                double angles[4];
                _feature_points.emplace_back(vlSift->keys[i].x, vlSift->keys[i].y);
                FeatureDescriptor descriptor;
                int angleCount = vl_sift_calc_keypoint_orientations(vlSift, angles, &vlSift->keys[i]);
                for(int j = 0; j < angleCount; ++j) {
                    Mat descriptor_array(1, SIFT_DESCRIPTOR_DIM, CV_32FC1);
                    vl_sift_calc_keypoint_descriptor(vlSift, (vl_sift_pix *) descriptor_array.data, &vlSift->keys[i], angles[j]);
                    descriptor.addDescriptor(descriptor_array);
                }
                _feature_descriptors.emplace_back(descriptor);
            }
        } while (vl_sift_process_next_octave(vlSift) != VL_ERR_EOF);
    }
    vl_sift_delete(vlSift);
}