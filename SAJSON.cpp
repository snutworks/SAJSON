//
//	SAJSON.cpp
//
// Copyright (c) 2013 Natural Style Co. Ltd.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "SuperAnimCommon.h"
#include <sstream>

#define SAM_VERSION 1
#define TWIPS_PER_PIXEL (20.0f)
#define LONG_TO_FLOAT (65536.0f)

#define FRAMEFLAGS_REMOVES		0x01
#define FRAMEFLAGS_ADDS			0x02
#define FRAMEFLAGS_MOVES		0x04
#define FRAMEFLAGS_FRAME_NAME	0x08

#define MOVEFLAGS_ROTATE		0x4000
#define MOVEFLAGS_COLOR			0x2000
#define MOVEFLAGS_MATRIX		0x1000
#define MOVEFLAGS_LONGCOORDS	0x0800


#ifndef max
#define max(x,y) (((x) < (y)) ? (y) : (x))
#endif



namespace SuperAnim{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation object definition
	
	class SuperAnimObject
	{
	public:
		int mObjectNum;
		int mResNum;
		SuperAnimTransform mTransform;
		Color mColor;
	};
	
	typedef std::vector<SuperAnimObject> SuperAnimObjectVector;
	typedef std::map<int, SuperAnimObject> IntToSuperAnimObjectMap;
	
	class SuperAnimImage
	{
	public:
		SuperAnimSpriteId mSpriteId;
		std::string mImageName;
		int mWidth;
		int mHeight;
		SuperAnimTransform mTransform;
	};
	typedef std::vector<SuperAnimImage> SuperAnimImageVector;
	
	class SuperAnimFrame
	{
	public:
		SuperAnimObjectVector mObjectVector;
	};
	typedef std::vector<SuperAnimFrame> SuperAnimFrameVector;
	typedef std::map<std::string, int> StringToIntMap;
	class SuperAnimLabel{
	public:
		std::string mLabelName;
		int mStartFrameNum;
		int mEndFrameNum;
	};
	typedef std::vector<SuperAnimLabel> SuperAnimLabelArray;
	class SuperAnimMainDef
	{
	public:
		SuperAnimFrameVector mFrames;
		int mStartFrameNum;
		int mEndFrameNum;
		int mAnimRate;
		SuperAnimLabelArray mLabels;
		int mX;
		int mY;
		int mWidth;
		int mHeight;
		SuperAnimImageVector mImageVector;
	};
	//////////////////////////////////////////////////////////////////////////////////////////////////

	typedef std::map<std::string, SuperAnimMainDef> SuperAnimMainDefMap;
	class SuperAnimDefMgr
	{
	private:
		SuperAnimMainDefMap mMainDefCache;
	private:
		SuperAnimDefMgr();
		~SuperAnimDefMgr();
		
		// std::string theSuperAnimFile include the absolute path
		bool LoadSuperAnimMainDef(const std::string &theSuperAnimFile);
	public:
		static SuperAnimDefMgr *GetInstance();
		static void DestroyInstance();

		// std::string theSuperAnimFile include the absolute path
		SuperAnimMainDef *Load_GetSuperAnimMainDef(const std::string &theSuperAnimFile);
		void UnloadSuperAnimMainDef(const std::string &theName);
	};


	SuperAnimSpriteId LoadSuperAnimSprite(std::string theSpriteName){
		return InvalidSuperAnimSpriteId;
	}
	
	void UnloadSuperSprite(SuperAnimSpriteId theSpriteId){
		// none
	}
}

unsigned char* GetFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize){
	unsigned char * pBuffer = NULL;
	*pSize = 0;
	do
	{
		// read the file from hardware
		FILE *fp = fopen(pszFileName, pszMode);
		if(!fp) break;
		
		fseek(fp,0,SEEK_END);
		*pSize = ftell(fp);
		fseek(fp,0,SEEK_SET);
		pBuffer = new unsigned char[*pSize];
		*pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
		fclose(fp);
	} while (0);
	return pBuffer;
}

unsigned long createRGBA(int r, int g, int b, int a) {
    return ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff);
}

int main(int argc, char* argv[]){
	if(argc != 2){
		printf("usage: SAJSON sam_path\n");
		return 1;
	}

	SuperAnim::SuperAnimMainDef* p = SuperAnim::SuperAnimDefMgr::GetInstance()->Load_GetSuperAnimMainDef(argv[1]);
	
	std::stringstream result;
	
	result << "{";
	
	result << "\"fps\":" << p->mAnimRate << ",";
	result << "\"x\":" << p->mX << ",";
	result << "\"y\":" << p->mY << ",";
	result << "\"w\":" << p->mWidth << ",";
	result << "\"h\":" << p->mHeight << ",";
	
	result << "\"imgs\":";
	result << "[";
	
	std::stringstream images;
	std::string imagesSeparator;
	
	for(SuperAnim::SuperAnimImageVector::const_iterator i=p->mImageVector.begin(); i!=p->mImageVector.end(); ++i){
		images << imagesSeparator;
		images << "{";
		images << "\"img\": \""	<< i->mImageName.c_str() << "\",";
//		images << "\"w\":"		<< i->mWidth << ",";
//		images << "\"h\":"	<< i->mHeight << ",";
		images << "\"t\":[";
		images << i->mTransform.mMatrix.m[0][0] << ",";
		images << i->mTransform.mMatrix.m[0][1] << ",";
		images << i->mTransform.mMatrix.m[0][2] << ",";
		images << i->mTransform.mMatrix.m[1][0] << ",";
		images << i->mTransform.mMatrix.m[1][1] << ",";
		images << i->mTransform.mMatrix.m[1][2] << ",";
		images << i->mTransform.mMatrix.m[2][0] << ",";
		images << i->mTransform.mMatrix.m[2][1] << ",";
		images << i->mTransform.mMatrix.m[2][2];
		images << "]";
		images << "}";

		imagesSeparator = ",";
	}
	
	result << images.str();
	result << "],";
	
	result << "\"sFrame\":" << p->mStartFrameNum << ",";
	result << "\"eFrame\":" << p->mEndFrameNum << ",";
	
	result << "\"frames\":[";
	
	std::stringstream frames;
	std::string framesSeparator;
	
	for(SuperAnim::SuperAnimFrameVector::const_iterator i=p->mFrames.begin(); i!=p->mFrames.end(); ++i){
		frames << framesSeparator;
		frames << "{";
		frames << "\"objects\":[";

		std::stringstream objects;
		std::string objectsSeparator;
		
		for(SuperAnim::SuperAnimObjectVector::const_iterator j=i->mObjectVector.begin(); j!=i->mObjectVector.end(); ++j){
			objects << objectsSeparator;
			objects << "{";
//			objects << "\"objNum\": "	<< j->mObjectNum << ",";
			objects << "\"resNum\": "		<< j->mResNum	 << ",";
			objects << "\"t\":";
			objects << "[";
			objects << j->mTransform.mMatrix.m[0][0] << ",";
			objects << j->mTransform.mMatrix.m[0][1] << ",";
			objects << j->mTransform.mMatrix.m[0][2] << ",";
			objects << j->mTransform.mMatrix.m[1][0] << ",";
			objects << j->mTransform.mMatrix.m[1][1] << ",";
			objects << j->mTransform.mMatrix.m[1][2] << ",";
			objects << j->mTransform.mMatrix.m[2][0] << ",";
			objects << j->mTransform.mMatrix.m[2][1] << ",";
			objects << j->mTransform.mMatrix.m[2][2];
			objects << "]";
			
			SuperAnim::Color clr = j->mColor;
			unsigned long color = createRGBA(clr.mRed, clr.mGreen, clr.mBlue, clr.mAlpha);
			
			if (color > 0) {
				objects << ",";
				objects << "\"c\":" << color;
			}
			
			objects << "}";
			
			objectsSeparator = ",";
		}
		
		frames << objects.str();
		frames << "]";
		frames << "}";
		
		framesSeparator = ",";
	}
	
	result << frames.str();
	result << "],";
	
	//////
	
	result << "\"labels\": [";
	
	std::stringstream labels;
	std::string labelsSeparator;
	
	for(SuperAnim::SuperAnimLabelArray::const_iterator i=p->mLabels.begin(); i!=p->mLabels.end(); ++i){
		labels << labelsSeparator;
		labels << "{";
		labels << "\"name\":\"" << i->mLabelName.c_str() << "\",";
		labels << "\"start\":" << i->mStartFrameNum << ",";
		labels << "\"end\":" << i->mEndFrameNum;
		labels << "}";
		
		labelsSeparator = ",";
	}
	
	result << labels.str();
	result << "]";
	result << "}";
	
	printf("%s", result.str().c_str());
	return 0;
}