#pragma once
#pragma comment(lib, "glfw3.lib")

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h> 
using namespace std;
#include "GLFWCallBackMgr.h"


class GLFWFrameWork
{ 
public:
	GLFWwindow* window = NULL;
	int w = 1600, h = 900;
	HWND hwnd;

	GLFWFrameWork()
	{		
	}
	 
	void Init() {
		if (!glfwInit()) {
			fprintf(stderr, "ERROR: could not start GLFW3\n");
			exit(1);
		}	
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(w, h, "Window", NULL, NULL);
	//	glfwSetWindowPos(window, mconfig.windowPosw, mconfig.windowPosh);
		
		if (!window) {
			fprintf(stderr, "ERROR: could not open window with GLFW3\n");
			glfwTerminate();
			exit(1);
		}
		//glfwMakeContextCurrent(window);
		hwnd = glfwGetWin32Window(window);

		glfwSetKeyCallback(window, GLFWCallBackMgr::key_callback);
		glfwSetMouseButtonCallback(window, GLFWCallBackMgr::mouse_button_callback);
		glfwSetCursorPosCallback(window, GLFWCallBackMgr::mouse_move_callback);
		glfwSetScrollCallback(window, GLFWCallBackMgr::scroll_callback);
		glfwSetWindowFocusCallback(window, GLFWCallBackMgr::focus_callback);
		glfwSetWindowIconifyCallback(window, GLFWCallBackMgr::iconify_callback);
		glfwSetCharCallback(window, GLFWCallBackMgr::char_callback);

		glfwSwapInterval(1);
	} 

	void KeyFunc(){
		GLFWCallBackMgr::key_func();
	}
	//void Loop() {

	//	while (!glfwWindowShouldClose(window)) {
	//		glfwPollEvents();

	//		glfwSwapBuffers(window);
	//		
	//	}
	//	glfwTerminate();

	//}
};
