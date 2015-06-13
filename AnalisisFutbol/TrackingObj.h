#include "config_file.h"
#include "Player.h"

/* CLASE ENCARGADA DEL SEGUIMIENTO DE LOS ELEMENTOS DEL CAMPO */
class TrackingObj {
private:
	/* LLEVA A CABO EL SEGUIMIENTO DE LOS JUGADORES  */
	static bool tracking(Mat frame, Mat filter, Point* pos, Player player);

	/* VENTANA DE B�SQUEDA DEL JUGADOR */
	static void searchWindow(Rect playerBox, Rect* searchWindow, Rect* relative);

public:

	/* COMPRUEBA SI EL PUNTO EST� EN EL FOCO DE LA C�MARA */
	static bool isInFocus(Point p);

	/* COMPRUEBA SI EL JUGADOR EST�N EN EL RANGO DENTRO DE LA IMAGEN */
	static bool isInRange(Rect* r);

	/* TRACKING DE LOS JUGADORES */
	static void trackPlayers(Player* player);

	/* DETECCI�N DE JUGADORES SOBRE EL CAMPO */
	static void objectDetection();

};
