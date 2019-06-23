var zIndex = 4;
var queryResult;
var userListeners = {
	"onQuery" : function (result) {
		console.log("============onQuery has no implementation, please confirm===========");
    },
    "online" : function () {
        console.log("============online has no implementation, please confirm===========");
    },
    "offline" : function () {
        console.log("============offline has no implementation, please confirm===========");
    },
	"lanOnline" : function (result) {
        console.log("============lanOnline has no implementation, please confirm===========");
    },
    "onlyLanOnline" : function (result) {
        console.log("============onlyLanOnline has no implementation, please confirm===========");
    },
	"onPost" : function (data) {
        console.log("============onPost has no implementation, please confirm===========");
    },
	"onOtaPost" : function (data) {
        console.log("============onOtaPost has no implementation, please confirm===========");
    },
	"onFault" : function (result) {
        console.log("============onFault has no implementation, please confirm===========");
    },
	"onMenuPost" : function (result) {
        console.log("============onMenuPost has no implementation, please confirm===========");
    },
	"onDeviceNameUpdate" : function (name) {
        console.log("============onDeviceNameUpdate has no implementation, please confirm===========");
    }
};

function CTSmartClient(listeners) {
	userListeners.onQuery = listeners.onQuery || userListeners.onQuery;
	userListeners.online = listeners.online || userListeners.online;
    userListeners.offline = listeners.offline || userListeners.offline;
    userListeners.lanOnline = listeners.lanOnline || userListeners.lanOnline;
    userListeners.onlyLanOnline = listeners.onlyLanOnline || userListeners.onlyLanOnline;
    userListeners.onPost = listeners.onPost || userListeners.onPost;
    userListeners.onOtaPost = listeners.onOtaPost || userListeners.onOtaPost;
    userListeners.onFault = listeners.onFault || userListeners.onFault;
    userListeners.onMenuPost = listeners.onMenuPost || userListeners.onMenuPost;
    userListeners.onDeviceNameUpdate = listeners.onDeviceNameUpdate || userListeners.onDeviceNameUpdate;

	var client = new Object();
	client.query = function (to, callback) {
		queryResult = stringToJson(window.cloud.query());
		userListeners.onQuery(queryResult);
        //如有故障，处理故障
		var fids = queryResult["fids"];
        if (fids != "" && fids != undefined) {
        	fault(fids);
		}
	}
	
	client.queryServer = function() {
		window.cloud.queryServer();
	}

	client.httpReq = function (url, method, params, needLogin, callback) {
		var result = window.cloud.httpReq(url, method, params, needLogin);
		if (callback != null) {
			callback(result);
		}
	}

	client.showMsg = function() {
		window.cloud.showMsg();
	}
	
	client.getDeviceName = function() {
		return window.cloud.getDeviceName();
	}

	client.getDeviceId = function() {
		return window.cloud.getDeviceId();
	}
	
	client.close = function() {
		window.cloud.close();
	}
	
	client.opt = function (to, dvid, value) {
		window.cloud.opt(dvid, value.toString());
	}
	
	client.shareFriend = function () {
		window.cloud.shareFriend();
	}
	
	client.shareDevice = function () {
		window.cloud.shareDevice();
	}
	
	client.editName = function () {
		window.cloud.editName();
	}
	
	client.updateWifi = function () {
		window.cloud.updateWifi();
	}
	
	client.deviceUsers = function () {
		window.cloud.deviceUsers();
	}
	
	client.deleteDevice = function () {
		window.cloud.deleteDevice();
	}
	
	client.upgrade = function () {
		window.cloud.upgrade();
	}
	/**
	 * 启动关于页面
	 * @param help 设备帮助页面url
	 * @param question 设备常见页面url
	 * @param showReset 是否显示固件升级
	 * @param showUpgrade 是否显示固件升级
     */
	client.about = function (help, question, showReset, showUpgrade) {
		window.cloud.about(help, question, showReset, showUpgrade);
	}

	client.showWindow = function (id, callback) {
		document.getElementById(id).style.display = "";
		document.getElementById(id).style.zIndex = ++zIndex;
		document.getElementById(id).style.left = 0;
		document.getElementById(id).style.top = 0;
		document.getElementById(id).style.position = "absolute";
		//        left:50%; top:50%;
		if (!document.getElementById("background").style.display == "") {
			document.getElementById("background").style.display = "";
		}
		callback;
	}
	client.closeWindow = function (id, callback) {
		--zIndex;
		document.getElementById(id).style.display = "none";
		document.getElementById(id).style.zIndex = "";
		if (divMap.isEmpty() || null == divMap || divMap.size() == 0) {
			document.getElementById("background").style.display = "none";
		}
		callback;
	}

    //打开一个新的web窗口
    client.openURL = function (url, needBackButton) {
        window.cloud.openURL(url, needBackButton);
    }
    //关闭当前web窗口
    client.closeURL = function () {
        window.cloud.closeURL();
    }
    //暂停菜谱
    client.menuStart = function () {
        window.cloud.menuStart();
    }
    //暂停菜谱
    client.menuNextStep = function () {
        window.cloud.menuNextStep();
    }
	//暂停菜谱
    client.menuPause = function () {
		window.cloud.menuPause();
    }
	//停止菜谱
    client.menuStop = function () {
        window.cloud.menuStop();
    }
    /**
	 * 我猜功能
     * @param key 本次猜标识，云端返回猜测结果时携带
     * @param product 猜测名称
     * @param value 猜测是否正确：1-正确，0-错误
     */
    client.guess = function (key, product, value) {
		window.cloud.guess(key, product, value);
    }
    /**
	 * 获取未读消息数目
     * @returns {*}
     */
    client.getUnreadMsgNum = function () {
        return window.cloud.getUnreadMsgNum();
    }
	return client;
}

function show(x) {
	alert(x);
}

function stringToJson(str) {
	try {
		str = str.replace(/\'/g, "\"");
		return JSON.parse(str);
	} catch (error) {
		console.log(error);
	}
}

function queryString(name) {
	return name;
}

function getCookieValue(name) {
	return name;
}

function updateDeviceName(name) {
	userListeners.onDeviceNameUpdate(name);
}

function getValue(dvid) {
	var saveAs = queryResult["as"];
	return saveAs[dvid];
}

function post(data) {
	if (userListeners) {
		var jsonData = stringToJson(data);
		userListeners.onPost(jsonData);
	}
}

function online() {
	if (userListeners) {
		userListeners.online();
	}
}

function lanOnline(result) {
	if (userListeners) {
		userListeners.lanOnline(result);
	}
}

function onlyLanOnline(result) {
	if (userListeners) {
		userListeners.onlyLanOnline(result);
	}
}

function offline() {
	if (userListeners) {
		userListeners.offline();
	}
}

function otaPost() {
	if (userListeners) {
		userListeners.onOtaPost();
	}
}
/**
 * app发生故障时使用
 * @param result 故障协议字符串
 */
function fault(result) {
    var res = stringToJson(result);
    //如有故障，处理故障
    var fids = res["fids"];
    if (fids != "" && fids != "undefined") {
        userListeners.onFault(fids);
    }
    else {
        userListeners.onFault("");
    }
}

function menuPost(result) {
    if (userListeners) {
        userListeners.onMenuPost(stringToJson(result));
    }
}


