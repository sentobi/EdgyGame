#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
#include "Vector3.h"

#include "ShaderProgram.h"

#include <irrKlang.h>

using namespace irrklang;

struct GLFWwindow;

class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();

    void InitDisplay(); // initialise the OpenGL environment here

	void UpdateInput();
	void PostInputUpdate();
	
	static void MouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallbacks(GLFWwindow* window, double xoffset, double yoffset);

	int GetWindowHeight();
	int GetWindowWidth();
	
    // sound engine
    static ISoundEngine *m_soundEngine;
    static ISound *Sound_Footstep;
    static ISound *Sound_BalloonDeflating;
    static ISound *Sound_GrenadeExplode;
    static ISound *Sound_BalloonPop;

    static void PlayBalloonDeflateSE(const Vector3& playerPos, const Vector3& playerTarget, const Vector3& balloonPos);
    static void PlayGrenadeExplodeSE(const Vector3& playerPos, const Vector3& playerTarget, const Vector3& explodePos);

private:
	Application();
	~Application();

	static bool IsKeyPressed(unsigned short key);

	// Should make these not hard-coded :P
	//const static int m_window_width = 800;
	//const static int m_window_height = 600;
    int m_window_width;
    int m_window_height;

	//Declare a window object
	StopWatch m_timer;

    // Pointer to shader program
    ShaderProgram* currProg;
};

#endif