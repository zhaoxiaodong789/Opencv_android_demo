#include "com_example_opencv_MainActivity.h"
#include <android/bitmap.h>
#include <opencv2/opencv.hpp>
#include <string>

#include <android/log.h>
#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace cv;
using namespace std;

//extern "C"
///**
// * 把一个jstring转换成一个c语言的char* 类型.
// */
//char* _JString2CStr(JNIEnv* env, jstring jstr) {
//    char* rtn = NULL;
//    jclass clsstring = (*env)->FindClass(env, "java/lang/String");
//    jstring strencode = (*env)->NewStringUTF(env,"GB2312");
//    jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
//    jbyteArray barr = (jbyteArray)(*env)->CallObjectMethod(env, jstr, mid, strencode); // String .getByte("GB2312");
//    jsize alen = (*env)->GetArrayLength(env, barr);
//    jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
//    if(alen > 0) {
//        rtn = (char*)malloc(alen+1); //"\0"
//        memcpy(rtn, ba, alen);
//        rtn[alen]=0;
//    }
//    (*env)->ReleaseByteArrayElements(env, barr, ba,0);
//    return rtn;
//}



extern "C" JNIEXPORT void
JNICALL Java_com_example_opencv_MainActivity_getEdge
        (JNIEnv *env, jobject obj, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels;

    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        Mat gray;
        cvtColor(temp, gray, COLOR_RGBA2GRAY);
        Canny(gray, gray, 45, 75);
        cvtColor(gray, temp, COLOR_GRAY2RGBA);
    } else {
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        Mat gray;
        cvtColor(temp, gray, COLOR_RGB2GRAY);
        Canny(gray, gray, 45, 75);
        cvtColor(gray, temp, COLOR_GRAY2RGB);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}

//int add(int a, int b){
//    return a+b;
//}
//
//extern "C"
//JNIEXPORT jstring JNICALL
//Java_com_example_opencv_MainActivity_stringFromJNI(
//        JNIEnv *env, jobject obj, jstring s) {
//    string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//    int a;
//    a = add(1,2);
//
//    char* fromJava = _JString2CStr(env,s);//I am form java add I am from C
//    //c:
//    char* fromC = "add I am from C";
//    //拼接函数strcat
//    strcat(fromJava,fromC);//把拼接的结果放在第一参数里面
//    //jstring     (*NewStringUTF)(JNIEnv*, const char*);
//    LOGD("fromJava===%s\n",fromJava);
//    return env->NewStringUTF(fromC);
//
//}

template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}


map<string, int> DECODING_TABLE = {
        {"0001101" , 0}, {"0100111" , 0}, {"1110010" , 0},
        {"0011001" , 1}, {"0110011" , 1}, {"1100110" , 1},
        {"0010011" , 2}, {"0011011" , 2}, {"1101100" , 2},
        {"0111101" , 3}, {"0100001" , 3}, {"1000010" , 3},
        {"0100011" , 4}, {"0011101" , 4}, {"1011100" , 4},
        {"0110001" , 5}, {"0111001" , 5}, {"1001110" , 5},
        {"0101111" , 6}, {"0000101" , 6}, {"1010000" , 6},
        {"0111011" , 7}, {"0010001" , 7}, {"1000100" , 7},
        {"0110111" , 8}, {"0001001" , 8}, {"1001000" , 8},
        {"0001011" , 9}, {"0010111" , 9}, {"1110100" , 9}
};

map<int, map<int, string>> EDGE_TABLE = {
        {2, {{2, "6"}, {3, "0"}, {4, "4"}, {5, "3"}}},
        {3, {{2, "9"}, {3, "33"}, {4, "34"}, {5, "5"}}},
        {4, {{2, "9"}, {3, "43"}, {4, "44"}, {5, "5"}}},
        {5, {{2, "6"}, {3, "0"}, {4, "4"}, {5, "3"}}}

};

const int INDEX_IN_WIDTH[13] = { 0, 4, 8, 12, 16, 20, 24, 33, 37, 41, 45, 49, 53 };

map<int, map<int, string>> FIRST_TABLE = {
        {2, {{2, "O"}, {3, "E"}, {4, "O"}, {5, "E"}}},
        {3, {{2, "E"}, {3, "O"}, {4, "E"}, {5, "O"}}},
        {4, {{2, "O"}, {3, "E"}, {4, "O"}, {5, "E"}}},
        {5, {{2, "E"}, {3, "O"}, {4, "E"}, {5, "O"}}}
};

map<string, int> FIRST_DETECT_TABLE = {
        {"OOOOOO" , 0}, {"OOEOEE" , 1}, {"OOEEOE" , 2}, {"OOEEEO" , 3}, {"OEOOEE" , 4},
        {"OEEOOE" , 5}, {"OEEEOO" , 6}, {"OEOEOE" , 7}, {"OEOEEO" , 8 },{"OEEOEO" , 9}
};


list<int> get_bar_space_width(Mat img) {
    int currentPix = -1;
    int lastPix = -1;
    int pos = 0;
    int i, j;
    list<int> width;
    Scalar s;
    for (j = 0; j < img.rows; j++) {
        width.clear();
        for (i = 0; i < img.cols; i++) {
            s = img.at<uchar>(j, i);
            currentPix = s[0];
            if (currentPix != lastPix) {
                if (lastPix == -1) {
                    lastPix = currentPix;
                    pos = i;
                }
                else {
                    width.push_back(i - pos);
                    pos = i;
                    lastPix = currentPix;
                }

            }


        }
        if (width.size() == 60)
            break;

    }
    return width;

}

int divide(int t, int l) {
    if (float(t) / l < 0.357) {
        return 2;
    }
    else if (float(t) / l < 0.500) {
        return 3;
    }
    else if (float(t) / l < 0.643) {
        return 4;
    }
    else {
        return 5;
    }
}

vector<int> cal_similar_edge(vector<int> data) {
    vector<int> similarEdge;
    /*
    float limit = 0;
    limit = float(data[1] + data[2] + data[3]) / 3 * 1.5;
    if (data[1] >= limit || data[2] >= limit || data[3] >= limit) {
        cout << "error!" << endl;
        return similarEdge;
    }
*/
    int index = 4;
    int T1 = 0, T2 = 0, L = 0;


    while (index < 54) {
        if (index == 28 || index == 29 || index == 30 || index == 31 || index == 32){
            index++;
            continue;
        }
        T1 = data[index] + data[index + 1];
        T2 = data[index + 1] + data[index + 2];
        L = data[index] + data[index + 1] + data[index + 2] + data[index + 3];
        similarEdge.push_back(divide(T1, L));
        similarEdge.push_back(divide(T2, L));
        index += 4;
    }


    return similarEdge;
}

void decode_sharp(vector<string>& barCode, vector<int> barSpaceWidth) {
    int i, index, c1, c2= 0;

    for (i = 0; i < 13; i++) {
        if (barCode[i] == "44") {
            index = INDEX_IN_WIDTH[i];
            c1 = barSpaceWidth[index + 2];
            c2 = barSpaceWidth[index + 3];
            if (c1 > c2) {
                barCode[i] = "1";
            }
            else {
                barCode[i] = "7";
            }
        }
        else if (barCode[i] == "33"){
            index = INDEX_IN_WIDTH[i];
            c1 = barSpaceWidth[index];
            c2 = barSpaceWidth[index + 1];
            if (c1 > c2) {
                barCode[i] = "2";
            }
            else {
                barCode[i] = "8";
            }
        }
        else if (barCode[i] == "34") {
            index = INDEX_IN_WIDTH[i];
            c1 = barSpaceWidth[index];
            c2 = barSpaceWidth[index + 1];
            if (c1 > c2) {
                barCode[i] = "7";
            }
            else {
                barCode[i] = "1";
            }
        }
        else if (barCode[i] == "43") {
            index = INDEX_IN_WIDTH[i];
            c1 = barSpaceWidth[index + 1];
            c2 = barSpaceWidth[index + 2];
            if (c1 > c2) {
                barCode[i] = "2";
            }
            else {
                barCode[i] = "8";
            }
        }
    }


}

bool check_bar_code(vector<int> barCode) {
    int evens, odds, sum = 0;
    evens = barCode[11] + barCode[9] + barCode[7] + barCode[5] + barCode[3] + barCode[1];
    odds = barCode[10] + barCode[8] + barCode[6] + barCode[4] + barCode[2] + barCode[0];
    sum = evens * 3 + odds;
    if (barCode[12] == (10 - sum % 10) % 10) {
        return true;
    }
    else
        return false;
}


vector<string> decode_similar_edge(vector<int> edge) {
    /*
    vector<string> barCode;
    barCode.push_back("6");
    int i = 0;
    for (i = 0; i < 24; i += 2) {
        barCode.push_back(EDGE_TABLE[edge[i]][edge[i+1]]);

    }
    */
    vector<string> barCode;
    string str;
    for (int i = 0; i < 12; i += 2){
        str = str + FIRST_TABLE[edge[i]][edge[i + 1]];

    }

    if (FIRST_DETECT_TABLE.find(str) == FIRST_DETECT_TABLE.end()) {
        return barCode;
    }
    else {
        string s = to_string(FIRST_DETECT_TABLE[str]);
        barCode.push_back(s);
    }

    for (int i = 0; i < 24; i += 2) {
        barCode.push_back(EDGE_TABLE[edge[i]][edge[i + 1]]);

    }


    return barCode;
}


Mat Ostu(Mat grayImg) {
    int grayImg_width, grayImg_height;
    grayImg_width = grayImg.cols;
    grayImg_height = grayImg.rows;

    Mat grayImg_1, grayImg_2, grayImg_3, grayImg_4, grayImg_5, grayImg_6;
    //grayImg_1 = grayImg(Rect(0, 0, int(grayImg_width / 2), int(grayImg_height / 2)));

    grayImg_1 = grayImg(Range(0, int(grayImg_height / 2)), Range(0, int(grayImg_width / 3)));
    grayImg_2 = grayImg(Range(0, int(grayImg_height / 2)), Range(int(grayImg_width / 3), int(grayImg_width / 3 * 2)));
    grayImg_3 = grayImg(Range(0, int(grayImg_height / 2)), Range(int(grayImg_width / 3 * 2), grayImg_width));
    grayImg_4 = grayImg(Range(int(grayImg_height / 2), grayImg_height), Range(0, int(grayImg_width / 3)));
    grayImg_5 = grayImg(Range(int(grayImg_height / 2), grayImg_height), Range(int(grayImg_width / 3), int(grayImg_width / 3 * 2)));
    grayImg_6 = grayImg(Range(int(grayImg_height / 2), grayImg_height), Range(int(grayImg_width / 3 * 2), grayImg_width));

    Mat binaryImg_1, binaryImg_2, binaryImg_3, binaryImg_4, binaryImg_5, binaryImg_6;

    threshold(grayImg_1, binaryImg_1, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    threshold(grayImg_2, binaryImg_2, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    threshold(grayImg_3, binaryImg_3, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    threshold(grayImg_4, binaryImg_4, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    threshold(grayImg_5, binaryImg_5, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    threshold(grayImg_6, binaryImg_6, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);

    Mat binaryImg;
    binaryImg = grayImg.clone();
    binaryImg_1.copyTo(binaryImg(Range(0, int(grayImg_height / 2)), Range(0, int(grayImg_width / 3))));
    binaryImg_2.copyTo(binaryImg(Range(0, int(grayImg_height / 2)), Range(int(grayImg_width / 3), int(grayImg_width / 3 * 2))));
    binaryImg_3.copyTo(binaryImg(Range(0, int(grayImg_height / 2)), Range(int(grayImg_width / 3 * 2), grayImg_width)));
    binaryImg_4.copyTo(binaryImg(Range(int(grayImg_height / 2), grayImg_height), Range(0, int(grayImg_width / 3))));
    binaryImg_5.copyTo(binaryImg(Range(int(grayImg_height / 2), grayImg_height), Range(int(grayImg_width / 3), int(grayImg_width / 3 * 2))));
    binaryImg_6.copyTo(binaryImg(Range(int(grayImg_height / 2), grayImg_height), Range(int(grayImg_width / 3 * 2), grayImg_width)));

    return binaryImg;
}

String barcode_detection(Mat temp){
    //13 识别
    String s;
    Mat imgGray, imageBinary;
    int i = 0;
    list<int> width;
    vector<int> barSpaceWidth(60);

    cvtColor(temp, imgGray, COLOR_RGBA2GRAY);

    //二值化
    //threshold(imgGray, imageBinary, 80, 255, CV_THRESH_BINARY);

    //ostu
    imageBinary = Ostu(imgGray);

    //提取条空宽度
    LOGE("条空宽度");
    width = get_bar_space_width(imageBinary);
    for (list<int>::iterator iter = width.begin(); iter != width.end(); ++iter, i++) {
        barSpaceWidth[i] = *iter;
        LOGE("%d", *iter);

    }



    //计算相似边数值
    vector<int> similarEdge = cal_similar_edge(barSpaceWidth);
    LOGE("计算相似边数值");
    for (vector<int>::iterator iter = similarEdge.begin(); iter != similarEdge.end(); ++iter, i++) {

        LOGE("%d", (*iter));

    }


    //相似边译码
    vector<string> barCode_string = decode_similar_edge(similarEdge);
    LOGE("相似边译码");
    for (vector<string>::iterator iter = barCode_string.begin(); iter != barCode_string.end(); ++iter, i++) {

        LOGE("%s", (*iter).c_str());

    }

    //针对‘#’译码
    decode_sharp(barCode_string, barSpaceWidth);
    LOGE("针对‘#’译码");
    for (vector<string>::iterator iter = barCode_string.begin(); iter != barCode_string.end(); ++iter, i++) {

        LOGE("%s", (*iter).c_str());

    }


    //将barCode的string
    vector<int> barCode;
    for (vector<string>::iterator iter = barCode_string.begin(); iter != barCode_string.end(); ++iter) {
        s = s + (*iter);
        //将barCode的string数组转为int
        barCode.push_back(atoi((*iter).c_str()));
    }

    LOGE("条形码译码：%s", s.c_str());



    //校验位
    bool t = check_bar_code(barCode);
    LOGE("校验位:%d", t);

    return s;

}


extern "C" JNIEXPORT jstring
JNICALL Java_com_example_opencv_MainActivity_getBarcode
        (JNIEnv *env, jobject obj, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels;
    string s;



    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        Mat gray;
//        cvtColor(temp, gray, COLOR_RGBA2GRAY);
//        Canny(gray, gray, 45, 75);
//        cvtColor(gray, temp, COLOR_GRAY2RGBA);

        s = barcode_detection(temp);

    } else {
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        Mat gray;
        cvtColor(temp, gray, COLOR_RGB2GRAY);
        Canny(gray, gray, 45, 75);
        cvtColor(gray, temp, COLOR_GRAY2RGB);

        //13 识别

        Mat imgGray, imageBinary;
        int i = 0;
        list<int> width;
        vector<int> barSpaceWidth(60);

        cvtColor(temp, imgGray, CV_RGB2GRAY);
        threshold(imgGray, imageBinary, 80, 255, CV_THRESH_BINARY);

        //提取条空宽度
        width = get_bar_space_width(imageBinary);
        for (list<int>::iterator iter = width.begin(); iter != width.end(); ++iter, i++) {
            barSpaceWidth[i] = *iter;


        }



        //计算相似边数值
        vector<int> similarEdge = cal_similar_edge(barSpaceWidth);




        //相似边译码
        vector<string> barCode_string = decode_similar_edge(similarEdge);

        //针对‘#’译码
        decode_sharp(barCode_string, barSpaceWidth);

//        //将barCode的string转换为int
//        vector<int> barCode;
//        for (vector<string>::iterator iter = barCode_string.begin(); iter != barCode_string.end(); ++iter) {
//            barCode.push_back(stoi(*iter));
//        }


        //将barCode的string数组转为string
        vector<int> barCode;

        for (vector<string>::iterator iter = barCode_string.begin(); iter != barCode_string.end(); ++iter) {
            s = s + (*iter);
            //将barCode的string数组转为int
            barCode.push_back(atoi((*iter).c_str()));
        }




        //校验位
        bool t = check_bar_code(barCode);
        LOGE("校验位:%d", t);
//        cout << check_bar_code(barCode) << endl;
//        cout << "--------------" << endl;


        //最终解码

//        for (vector<int>::iterator iter = barCode.begin(); iter != barCode.end(); ++iter) {
//            cout << *iter;
//        }

    }
    AndroidBitmap_unlockPixels(env, bitmap);
    //return s;
    return env->NewStringUTF(s.c_str());

}

