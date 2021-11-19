// Contains an Image Event Handler that prints a message for each event method call.

#ifndef INCLUDED_IMAGEEVENTPRINTER_H_7884943
#define INCLUDED_IMAGEEVENTPRINTER_H_7884943

#include <pylon/ImageEventHandler.h>
#include <pylon/GrabResultPtr.h>
#include <iostream>

// ����OpenCV API
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>



namespace Pylon
{
    class CInstantCamera;

    class CImageEventPrinter : public CImageEventHandler
    {
    public:

        virtual void OnImagesSkipped( CInstantCamera& camera, size_t countOfSkippedImages)
        {
            std::cout << "OnImagesSkipped event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
            std::cout << countOfSkippedImages  << " images have been skipped." << std::endl;
            std::cout << std::endl;
        }


        virtual void OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
        {
            std::cout << "OnImageGrabbed event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
               /* std::cout << "SizeX: " << ptrGrabResult->GetWidth() << std::endl;
                std::cout << "SizeY: " << ptrGrabResult->GetHeight() << std::endl;
                const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
                std::cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << std::endl;
                std::cout << std::endl;*/

				// ��ȡͼ������
				std::cout << "SizeX: " << ptrGrabResult->GetWidth() << std::endl;
				std::cout << "SizeY: " << ptrGrabResult->GetHeight() << std::endl;

				//��ץȡ�Ļ�������ת����pylon image.
				//formatConverter.Convert(pylonImage, ptrGrabResult);

				// �� pylon imageת��OpenCV image.
				//openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());

				//�����Ҫ����ͼƬѡ��if (saveImages)������ͼѡ��if (!saveImages)    #define saveImages 1
				//if (saveImages)
				//if (!saveImages)
				//{
				//	std::ostringstream s;
				//	// �����������ļ����洢ͼƬ
				//	s << "image_" << grabbedlmages << ".jpg";
				//	std::string imageName(s.str());
				//	//����OpenCV image.
				//	//imwrite("D:/data/data_basler/" + imageName, openCvImage);
				//	imwrite("C:/Users/46263/Desktop/BaslerSaveimage/" + imageName, openCvImage);
				//	//imwrite(imageName, openCvImage);//ֱ�Ӵ��ڵ�ǰĿ¼������Դ�ļ����ļ����£�
				//	grabbedlmages++;
				//}

            }
            else
            {
                std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
            }
        }
    };
}

#endif /* INCLUDED_IMAGEEVENTPRINTER_H_7884943 */
