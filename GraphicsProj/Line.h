#ifndef LINE_H_
#define LINE_H_

#include <list>

#include <opencv2\core\core.hpp>

namespace proj
{
	class Line
	{
	public:
		Line() : mPoints(), mPointIter() {}
		virtual ~Line() { }
		
		void addPoint(cv::Point point) { mPoints.push_back(point); }
		int getLineNumbers() { return (int)mPoints.size() - 1; }
		cv::Point getCurrentPoint() { return *mPointIter; }
		cv::Point getNextPoint() { return *(++mPointIter); }
		void resetListIter() { mPointIter = mPoints.begin(); }
		
	private:
		std::list<cv::Point> mPoints;
		std::list<cv::Point>::iterator mPointIter;
	};
}

#endif