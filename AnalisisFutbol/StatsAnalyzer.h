#include "config_file.h"

class StatsAnalyzer
{
private:

	// ESTADÍSTICAS
	static vector<vector<Mat>> playerAreaStats;
	static Mat teamAreaStats;

	/* NORMALIZA LOS VALORES DE LA MATRIZ ENTRE 0 Y 1 */
	static Mat normalizeAreaStats(Mat m);

	/* CALCULA LA DISTANCIA ENTRE DOS PUNTOS */
	static float distance(Point actualPoint, Point lastPoint);

public:

	/* INCREMENTA EL VALOR EN LA POSICIÓN INDICADA */
	static void addPosition(Mat m, Point p);

	/* INCREMENTA LA DISTANCIA RECORRIDA Y ACTUALIZA LA VELOCIDAD */
	static void addDistanceAndSpeed(float* distance, Point actualPoint, Point lastPoint,
		                            float* speed, int* nSpeed, float* maxSpeed);

	/* DEVUELVE LAS ESTADÍSTICAS DE POSICIONES */
	static Mat getAreaStats(Mat m);

	/* CALCULA TODAS LAS ESTADÍSTICAS */
	static void calculateAllStats();

	/* DETERMINA SI DOS PUNTOS SE CORRESPONDEN CON EL MISMO ELEMENTO */
	static bool isSamePoint(Point p1, Point p2);

};

