#include "TrackingObj.h"
#include "GUI.h"

void TrackingObj::trackObject(Mat filtro, Mat &partido) {
	Mat temp;
	filtro.copyTo(temp);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);
	double refArea=0;
	if(hierarchy.size() > 0) {
		int numObjects = hierarchy.size();

		PlayerClassifier::clearVectors();

		rectangle(filtro, Point(0,0), Point(MAX_BALL_SIZE,MAX_BALL_SIZE), Scalar(0), 0);
		rectangle(filtro, Point(0,0), Point(7,7), Scalar(0), 0);

		for(int index = 0; index >= 0; index = hierarchy[index][0]) {

			vector<Rect> minRect( contours.size() );
			for( int i = 0; i < contours.size(); i++ ) {
				minRect[i] = boundingRect( Mat(contours[i]) );
			}
			if( (minRect[index].width>GUI::MIN_WIDTH && minRect[index].width<GUI::MAX_WIDTH) &&
				(minRect[index].height>GUI::MIN_HEIGH && minRect[index].height<GUI::MAX_HEIGH) ) {

				PlayerClassifier::comparePlayer(partido,filtro,minRect[index]);

				PlayerClassifier::findAndDraw(minRect[index], partido);

			} else if(	minRect[index].width < MAX_BALL_SIZE &&
						minRect[index].height < MAX_BALL_SIZE) {
				rectangle(partido,minRect[index],Scalar(0,0,255),-1);
			}
		}

		PlayerClassifier::sortVectors();

		PlayerClassifier::drawTeams(partido);
	}
}
