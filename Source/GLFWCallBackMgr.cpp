#include "GLFWCallBackMgr.h"

map<string,KeyCallBack> GLFWCallBackMgr::KeyCallBacks;
map<string, KeyFunc>	GLFWCallBackMgr::KeyFuncs;
map<string, MouseButtonCallBack> GLFWCallBackMgr::MouseButtonCallBacks;
map<string, MouseMoveCallBack>	GLFWCallBackMgr::MouseMoveCallBacks;
map<string, ScrollCallBack>	GLFWCallBackMgr::ScrollCallBacks;
map<string, CharCallBack>	GLFWCallBackMgr::CharCallBacks;
map<string, FocusCallBack>	GLFWCallBackMgr::FocusCallBacks;
map<string, IconifyCallBack>	GLFWCallBackMgr::IconifyCallBacks;
