#pragma once
#include <GLFW/glfw3.h>
#include <map>
#include <functional>
using namespace std;

using KeyCallBack = function<void(GLFWwindow*,int ,int ,int, int)>;
using KeyFunc = function<void(void)>;
using MouseButtonCallBack = function<void(GLFWwindow*, int, int, int)>;
using MouseMoveCallBack = function<void(GLFWwindow*, double, double)>;
using ScrollCallBack= function<void(GLFWwindow*, double, double)>;
using CharCallBack = function<void(GLFWwindow*, unsigned)>;
using FocusCallBack = function<void(GLFWwindow*, int)>;
using IconifyCallBack = function<void(GLFWwindow*, int)>;

class GLFWCallBackMgr
{
public:
	static void key_func() {
		for (auto& it : KeyFuncs) {
			it.second();
		}
	}
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		for (auto& it : KeyCallBacks) {
			it.second(window,key, scancode,action, mode);
		}
	}
	static void mouse_button_callback(GLFWwindow* window, int button, int  action, int mode) {
		for (auto& it : MouseButtonCallBacks) {
			it.second(window, button, action, mode);
		}
	}
	static void mouse_move_callback(GLFWwindow* window, double x, double y) {
		for (auto& it : MouseMoveCallBacks) {
			it.second(window,x,y);
		}
	}
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		for (auto& it : ScrollCallBacks) {
			it.second(window,xoffset,yoffset);
		}
	}
	static void char_callback(GLFWwindow* window, unsigned ch) {
		for (auto& it : CharCallBacks) {
			it.second(window,ch);
		}
	}
	static void iconify_callback(GLFWwindow* window, int x) {
		for (auto& it : IconifyCallBacks) {
			
			it.second(window, x);
		}
	}
	static void focus_callback(GLFWwindow* window, int x) {
		for (auto& it : FocusCallBacks) {
			it.second(window,x);
		}
	}

	static map<string,KeyCallBack> KeyCallBacks;
	static map<string, KeyFunc>	KeyFuncs;
	static map<string, MouseButtonCallBack> MouseButtonCallBacks;
	static map<string, MouseMoveCallBack>	MouseMoveCallBacks;
	static map<string, ScrollCallBack>	ScrollCallBacks;
	static map<string, CharCallBack>	CharCallBacks;
	static map<string, FocusCallBack>	FocusCallBacks;
	static map<string, IconifyCallBack>	IconifyCallBacks;


};

