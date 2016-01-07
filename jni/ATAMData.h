/*!
@file		ATAMData.h
@brief		All data for ATAM
*/

#pragma once

//#include "DirectoryConfig.h"
#include <opencv2/core.hpp>

#include <vector>
#include <list>
#include <map>
#include <mutex>

#ifdef SHOWLOG
#define LOGOUT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define LOGOUT(fmt, ...)
#endif

/*!
@struct		sPose
@brief		Pose parameters
*/
struct sPose
{
	sPose(void);
	sPose(const sPose& r);

	sPose& operator=(const sPose& r);

	void getM(cv::Mat &M) const;
	void getR(cv::Mat &R) const;
	void setFromM(const cv::Mat &M);
	void print(void) const;

	cv::Mat rvec;		//!< 3x1 rotation vector
	cv::Mat tvec;		//!< 3x1 translation vector
};

/*!
@struct		sTrack
@brief		point track
*/
struct sTrack
{
	sTrack(void);

	std::vector<cv::Point2f> vpt;	//!< list of points
	cv::KeyPoint kpt;				//!< keypoint at first frame
	int ptID;						//!< point ID
};

/*!
@struct		sKeyframe
@brief		keyframe data for mapping
*/
struct sKeyframe
{
	void clear(void);

	cv::Mat img;	//!< image
	sPose pose;		//!< pose

	// for relocalization
	std::vector<int> vkptID;			//!< point ID
	std::vector<cv::KeyPoint> vkpt;		//!< keypoints
	cv::Mat vdesc;						//!< descriptors

	// for ba
	std::vector<int> vptID;			//!< point ID
	std::vector<cv::Point2f> vpt;	//!< points

	int ID;		//!< keyframe ID
};

/*!
@struct		sBAData
@brief		data for BA
*/
struct sBAData
{
	void clear(void);

	std::vector<cv::Point3f> vpt3d;		//!< mapped points
	std::vector<int> vvisibleID;		//!< IDs of visible mapped points
	std::vector<sKeyframe> vkeyframe;	//!< keyframes
	std::vector<int> vkeyframeID;		//!< IDs of keyframes
};

/*!
@class		CMapData
@brief		map data
*/
class CMapData
{
public:
	CMapData(void);

	void Clear(void);
	bool CopytoBA(sBAData &data);
	void CopyfromBA(const sBAData &data);
	void AddKeyframe(const sKeyframe &kf, const cv::Mat &vdesc);
	sKeyframe& GetLastKeyframe(void);
	void UpdateLastKeyframe(const std::vector<cv::Point3f> &vpt3d, const std::vector<cv::KeyPoint> &vkpt, const cv::Mat &vdesc, std::vector<int> &vid);
	const cv::Point3f& GetPoint(const int id) const;
	std::vector<cv::Point3f> GetAllPoint();
	const sKeyframe& GetNearestKeyframe(const sPose &pose) const;

	void GetPoseforRelocalization(sPose &pose) const;
	void GetGoodPoseforRelocalization(sPose &pose) const;
	int GetSize(void) const;

private:
	std::vector<cv::Point3f> mvPt;	//!< 3D points in map
	std::vector<sKeyframe> mvKf;	//!< keyframes
	std::mutex mMapMutex;				//!< mutex
	bool mAdded;					//!< keyframe added and not used in BA
};

/*!
@struct		sATAMParams
@brief		parameters for ATAM
*/
struct sATAMParams
{
	sATAMParams();

	void loadParams(const std::string &name);

	int MAXPTS;					//!< maximum number of points
	int LEVEL;					//!< pyramid level for detector
	float DESCDIST;				//!< max distance for correct matches

	double BASEANGLE;			//!< angle (degree) for adding new keyframe
	double BASETAN;				//!< tangent of BASEANGLE
	int BAKEYFRAMES;			//!< number of keyframes for local bundle adjustment

	float PROJERR;				//!< tolerance for reprojection error
	int MINPTS;					//!< minimum number of points for tracking and mapping
	int PATCHSIZE;				//!< patch size for KLT
	float MATCHKEYFRAME;		//!< inlier ratio for matching keyframe

	float GOODINIT;				//!< inlier ration for initialization
	int RELOCALHIST;			//!< check last n frames for relocalization

	bool USEVIDEO;				//!< use video or not
	std::string VIDEONAME;		//!< video file name

	std::string CAMERANAME;		//!< camera parameter file name
};

/*!
@struct		sATAMData
@brief		ATAM Data
*/
struct sATAMData
{
	sATAMData(void);

	void clear(void);
	void clearTrack(void);
	void clearTrack(const int ID);
	void addTrack(const sTrack &in);

	cv::Mat previmg;						//!< previous image
	std::list<sTrack> vtrack;				//!< all tracks
	std::vector<cv::Point2f> vprevpt;		//!< tracked points in previous image
	double quality;							//!< tracking quality

	CMapData map;							//!< data for mapping
	sBAData baData;							//!< data for BA

	cv::Mat A, D;							//!< camera parameters
	double focal;							//!< focal length

	double scale;							//!< scale (world/local)
	cv::Mat transMat;						//!< transformation from local to world
	bool havescale;							//!< scale is computed or not

	std::vector<std::pair<sPose, sPose> > vposePair;	//!< set of world and local coordinates
	std::map<int, cv::Point3f> vtarget;				//!< target points

	std::vector<cv::KeyPoint> vKpt;			//!< keypoints in current image
};

const int NOID = -1;		//!< NO ID
const int DISCARD = -2;		//!< point that will be discarded