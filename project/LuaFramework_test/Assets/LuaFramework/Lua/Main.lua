--主入口函数。从这里开始lua逻辑
function Main()					
	 LuaFramework.Util.Log("Hello zhangxs");	
end

--场景切换通知
function OnLevelWasLoaded(level)
	Time.timeSinceLevelLoad = 0
end