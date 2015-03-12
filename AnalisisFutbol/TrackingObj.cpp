#include "GUI.h"
#include "TrackingObj.h"
#include "PlayerClassifier.h"
#include "From3DTo2D.h"
#include "GlobalStats.h"
#include "VideoManager.h"

/* COMPRUEBA SI EL JUGADOR EST�N EN EL RANGO DENTRO DE LA IMAGEN */
bool TrackingObj::isInRange(Rect* r) {
	Rect rect = *r;
	bool inRange = false;
	inRange = rect.tl().x >= 0 && rect.tl().y >= 0 && rect.br().x < VIDEO_WIDTH && rect.br().y < VIDEO_HEIGHT;
	if(!inRange) {
		int tl_x = rect.tl().x,tl_y = rect.tl().y,br_x = rect.br().x,br_y = rect.br().y;
		if(rect.tl().x < 0) {
			tl_x = 0;
		}
		if(rect.tl().y < 0) {
			tl_y = 0;
		}
		if(rect.br().x >= VIDEO_WIDTH) {
			br_x = VIDEO_WIDTH-1;
		}
		if(rect.br().y >= VIDEO_HEIGHT) {
			br_y = VIDEO_HEIGHT-1;
		}

		Rect tmp_rect = Rect(Point(tl_x,tl_y),Point(br_x,br_y));
		if(tmp_rect.width >= MIN_WIDTH && tmp_rect.height >= MIN_HEIGH) {
			inRange = true;
			*r = tmp_rect;
		}
	}
	return inRange;
}


/* COMPRUEBA SI EL PUNTO EST� EN EL FOCO DE LA C�MARA */
bool TrackingObj::isInFocus(Point p) {
	return p.x >= 0 && p.y >= 0 && p.x <= VIDEO_WIDTH && p.y <= VIDEO_HEIGHT;
}

/* VENTANA DE B�SQUEDA DEL JUGADOR */
void TrackingObj::searchWindow(Rect playerBox, Rect* searchWindow, Rect* relative) {
	Point windowP = Point(SEARCH_WINDOW);
	*searchWindow =  Rect(playerBox.tl()-windowP,playerBox.br()+windowP);
	*relative = Rect(windowP, playerBox.size());
	bool inRange = searchWindow->tl().x >= 0 && searchWindow->tl().y >= 0 &&
		searchWindow->br().x < VIDEO_WIDTH && searchWindow->br().y < VIDEO_HEIGHT;
	if(!inRange) {
		int tl_x = searchWindow->tl().x,tl_y = searchWindow->tl().y;
		int br_x = searchWindow->br().x,br_y = searchWindow->br().y;
		bool relativeChanged = false;
		Point relP = windowP;
		if(searchWindow->tl().x < 0) {
			relativeChanged = true;
			relP.x = playerBox.x;
			tl_x = 0;
		}
		if(searchWindow->tl().y < 0) {
			relativeChanged = true;
			relP.y = playerBox.y;
			tl_y = 0;
		}
		if(searchWindow->br().x >= VIDEO_WIDTH) {
			br_x = VIDEO_WIDTH-1;
		}
		if(searchWindow->br().y >= VIDEO_HEIGHT) {
			br_y = VIDEO_HEIGHT-1;
		}

		if(relativeChanged) {
			*relative = Rect(Point(relP), playerBox.size());
		}
		*searchWindow = Rect(Point(tl_x,tl_y),Point(br_x,br_y));
	}
}

/* LLEVA A CABO EL SEGUIMIENTO DE LOS JUGADORES  */
bool TrackingObj::tracking(Mat hsv, Mat filter, Mat* paint, Point* pos) {
	
	float rango[] = {0,255};
    const float* ranges [] = {rango};
    int channels [] = {0};
    int histSize [] = {128};

    Rect playerBox = Rect((*pos).x-PLAYER_WIDTH/2,(*pos).y-PLAYER_HEIGHT,PLAYER_WIDTH,PLAYER_HEIGHT);

	bool inRange = isInRange(&playerBox);

	if(inRange) {

		Mat hsv_roi = hsv(playerBox);

		Mat mask = filter(playerBox);
		Mat roi_hist;
		Mat images [] = {hsv_roi};

		calcHist(&hsv_roi,1,channels,mask,roi_hist,1,histSize,ranges);
		normalize(roi_hist,roi_hist,0,255,NORM_MINMAX);

		TermCriteria term_crit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 5, 1);

		//int team_id = 0;
		//Player player(team_id);

		Mat dst;
		Rect searchWindowRect, relativePlayerBox;
		searchWindow(playerBox, &searchWindowRect, &relativePlayerBox);
		Mat window = hsv(searchWindowRect);
		Mat windowMask = filter(searchWindowRect);
		calcBackProject(&window,1,channels,roi_hist,dst,ranges,1);
		dst&=windowMask;
		Rect tmp = relativePlayerBox;
		meanShift(dst,relativePlayerBox,term_crit);

		Point desp = relativePlayerBox.tl() - tmp.tl();
		

		//rectangle(*paint,Rect(playerBox.tl(),playerBox.size()),Scalar(255,255,255),1);

		//if ((int)video.get(CV_CAP_PROP_POS_FRAMES)%5 == 0) {
		//Point bcenter = Point(playerBox.tl().x + playerBox.width/2, playerBox.br().y);
		//player.addPosition(bcenter);
		//From3DTo2D::paint2DPositions(playerBox,D_SQ_NUM,paint);
		//}

		*pos = Point(playerBox.tl().x + desp.x + playerBox.width/2,playerBox.br().y + desp.y);
	}
	return inRange;
}

/* TRACKING DE LOS JUGADORES */
void TrackingObj::trackPlayers(Mat frame[N_VIDEOS], Mat filter[N_VIDEOS], Point* detectedPlayer, int index) {
	bool detected = false;
	Point newPos;
	vector<Point> positions;
	Mat paint[N_VIDEOS];
	for(int i=0; i<N_VIDEOS; i++) {
		//cvtColor(frame[i],paint[i],CV_HSV2BGR);
		Point* realPos = &From3DTo2D::getRealPosition(*detectedPlayer,i);
		if(isInFocus(*realPos)) {
			detected |= tracking(frame[i],filter[i],&paint[i],realPos);
			positions.push_back(From3DTo2D::get2DPosition(*realPos,i));
		}
	}
	/*Mat imm=VideoManager::joinSequences(paint);
	pyrDown(imm, imm, Size(imm.cols/2, imm.rows/2));
	pyrDown(imm, imm, Size(imm.cols/2, imm.rows/2));
	imshow("OLAKASE",imm);
	waitKey();*/
	if(detected) {
		for(int i=0; i<positions.size(); i++) {
			newPos+=positions.at(i);
		}
		newPos = Point(newPos.x/positions.size(), newPos.y/positions.size());
	} else {
		GlobalStats::playersToDelete.push_back(index);
	}
	*detectedPlayer = newPos;
}

/* REALIZA EL SEGUIMIENTO DE LOS ELEMENTOS DEL PARTIDO */
void TrackingObj::objectDetection(Mat filtro, Mat &partido, int nVideo, Mat paint) {
	Mat temp;		// Matriz auxiliar en la que buscaremos los contornos
	filtro.copyTo(temp);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);
	if(hierarchy.size() > 0) {													// Si se encuentra alg�n contorno
		for( int i = 0; i < contours.size(); i++ ) {							// Recorremos los contornos
			Rect elem = boundingRect(Mat(contours[i]));							// Creamos el boundingBox
			if(PlayerClassifier::isPlayerSize(elem)) {
				GlobalStats::locations[nVideo].push_back(elem);			// A�adimos al vector de elementos encontrados
			}
		}
	}
}
