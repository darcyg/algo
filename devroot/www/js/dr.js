$(function(){ 
	var content; 

	function DoRequest(path, arg, succ, err, comp) {
		$.ajax({
			url : path,
			type : 'POST',
			async: true,
			data: arg,
			timeout : 5000,
			//dataType: 'json',
			//dataType: 'html',
			dataType: 'html',
			beforeSend: function(xhr) {
			},
			success: function(data, textStatus, jqXHR) {
				succ(data);
			},
			error: function(xhr, textStatus) {
				err(textStatus);
			},
			complete: function(xhr, textStatus) {
				comp(textStatus);
			}
		});
	}


	function tblinit() {
		//$("#content tr:odd").css("background-color","#D2B48C"); 
		//$("#content tr:even").css("background-color","#C0C0C0"); 
		/*
		$("#content .disarea .table td").not(".op").click(function(){ 
			var clickObj = $(this); 
			content = clickObj.html(); 
			changeToEdit(clickObj); 
		}); 
		*/

		function changeToEdit(node){ 
			node.html(""); 

			var inputObj = $("<input type='text'/>"); 

			inputObj.css("border","0").css("background-color",node.css("background-color")) 
			.css("font-size",node.css("font-size")).css("height",25).css("margin",0).css("padding", 0)
			.css("border", "none")
			.css("width",node.css("width")).val(content).appendTo(node) 
			.get(0).select(); 

			inputObj.click(function(){ 
				return false; 
			}).keyup(function(event){ 
				var keyvalue = event.which; 
				if(keyvalue==13){ 
					node.html(node.children("input").val()); 
				} 
				if(keyvalue==27){ 
					node.html(content); 
				} 
			}).blur(function(){ 
				if(node.children("input").val()!=content){ 
					//if(confirm("是否保存修改的内容？","Yes","No")){ 
						node.html(node.children("input").val()); 
					//} else { 
						//node.html(content); 
					//} 
				}else{ 
					node.html(content); 
				} 
			}); 
		}

		$(".del").click(function() {
			var clickObj = $(this);
			var btntxt = clickObj.html();


			var curr = $("#curr").html();
			curr = curr.substring(0,curr.search("/"));
			var tbl  = $("#tblname option:selected").val();
			var arg  = {"curr" : curr, "tblname" : tbl};
			$(clickObj).parent().parent().children().not(".op").each(function(i,item) {
				arg["arg"+i] = $(item).html();
			});

			//alert(JSON.stringify(arg));

			DoRequest("/del", arg, function(data) { //success
				//console.log("response:" + data);
				$("#moditbl").html(data);
				$("#curr").html($("#aurr").html());
				tblinit();
			}, function(stat) { //error
				console.log("Requst /next Error:" + stat);
			}, function(stat) {
				console.log("Requst /next Complete!");
			});

		});

		$(".mod").click(function() {
			var clickObj = $(this);
			var btntxt = clickObj.html();

			if (btntxt == "修改") {
				$(clickObj).html("修改..");
				$(clickObj).parent().parent().children("td").not(".op").click(function(){ 
					var me = $(this); 
					content = me.html(); 
					changeToEdit(me); 
				}); 
			} else {
				if(confirm("是否保存修改的内容？","Yes","No")){ 
					var curr = $("#curr").html();
					curr = curr.substring(0,curr.search("/"));
					var tbl  = $("#tblname option:selected").val();
					var start = $(clickObj).parent().parent().index();
					var arg  = {"curr" : curr, "tblname" : tbl, "start" : start};
					$(clickObj).parent().parent().children().not(".op").each(function(i,item) {
						arg["arg"+i] = $(item).html();
					});


					//alert(JSON.stringify(arg));

					DoRequest("/mod", arg, function(data) { //success
					//console.log("response:" + data);
						$("#moditbl").html(data);
						$("#curr").html($("#aurr").html());
						tblinit();
					}, function(stat) { //error
						console.log("Requst /next Error:" + stat);
					}, function(stat) {
						console.log("Requst /next Complete!");
					});
				} else { 
					var curr = $("#curr").html();
					curr = curr.substring(0,curr.search("/"));
					var tbl  = $("#tblname option:selected").val();
					var arg  = {"curr" : curr, "tblname" : tbl};

					//alert(JSON.stringify(arg));

					DoRequest("/curr", arg, function(data) { //success
					//console.log("response:" + data);
						$("#moditbl").html(data);
						$("#curr").html($("#aurr").html());
						tblinit();
					}, function(stat) { //error
						console.log("Requst /curr Error:" + stat);
					}, function(stat) {
						console.log("Requst /curr Complete!");
					});
				}
				$(clickObj).html("修改");
				$(clickObj).parent().parent().children("td").not(".op").attr('onclick', '').unbind('click');
			}
		});



	}

	function modinit() {
		$("#prev").click(function() {
				var clickObj = $(this);
				var btntxt = clickObj.html();

				var curr = $("#curr").html();
				curr = curr.substring(0,curr.search("/"));
				var tbl  = $("#tblname option:selected").val();
				var arg  = {"curr": curr, "tblname" : tbl};


				DoRequest("/prev", arg, function(data) { //success
					//console.log("response:" + data);
					$("#moditbl").html(data);
					$("#curr").html($("#aurr").html());
					tblinit();
					}, function(stat) { //error
					console.log("Requst /Prev Error:" + stat);
					}, function(stat) {
					console.log("Requst /Prev Complete!");
					});
				});

		$("#next").click(function() {
				var clickObj = $(this);
				var btntxt = clickObj.html();

				var curr = $("#curr").html();
				curr = curr.substring(0,curr.search("/"));
				var tbl  = $("#tblname option:selected").val();
				var arg  = {"curr": curr, "tblname" : tbl};

				DoRequest("/next", arg, function(data) { //success
					//console.log("response:" + data);
					$("#moditbl").html(data);
					$("#curr").html($("#aurr").html());
					tblinit();
					}, function(stat) { //error
					console.log("Requst /next Error:" + stat);
					}, function(stat) {
					console.log("Requst /next Complete!");
					});
				});

		$("#add").click(function() {
				var clickObj = $(this);
				var btntxt = clickObj.html();

				var curr = $("#curr").html();
				curr = curr.substring(0,curr.search("/"));
				var tbl  = $("#tblname option:selected").val();
				var arg  = {"curr": curr, "tblname" : tbl};

				DoRequest("/add", arg, function(data) { //success
					//console.log("response:" + data);
					$("#moditbl").html(data);
					$("#curr").html($("#aurr").html());
					tblinit();
					}, function(stat) { //error
					console.log("Requst /next Error:" + stat);
					}, function(stat) {
					console.log("Requst /next Complete!");
					});

				});
		$("#tblname").change(function() {
				var clickObj = $(this);
				var btntxt = clickObj.html();

				//var curr = $("#curr").html();
				//curr = curr.substring(0,curr.search("/"));
				var curr = 1;
				var tbl  = $("#tblname option:selected").val();
				var arg  = {"curr" : curr, "tblname" : tbl};

				//alert(JSON.stringify(arg));

				DoRequest("/curr", arg, function(data) { //success
					//console.log("response:" + data);
					$("#moditbl").html(data);
					$("#curr").html($("#aurr").html());
					tblinit();
					}, function(stat) { //error
					console.log("Requst /curr Error:" + stat);
					}, function(stat) {
					console.log("Requst /curr Complete!");
					});
		});

		tblinit();
		$("#curr").html($("#aurr").html());
	}


	$("#menu ul li").click(function() {
		var page = $(this).index();
		var arg  = {"page" : page};

		DoRequest("/menu", arg, function(data) { //success
		//console.log("response:" + data);
			$("#content").html(data);
			//$("#curr").html($("#aurr").html());
			//tblinit();

			$("#content .disarea").css("height",$("#menu").css("height"));
			modinit();
		}, function(stat) { //error
			console.log("Requst /menu Error:" + stat);
		}, function(stat) {
			console.log("Requst /menu Complete!");
		});
	});

	$(window).resize(function() {
		$("#content .disarea").css("height",$("#menu").css("height"));
		modinit();
	});


	$("#content .disarea").css("height",$("#menu").css("height"));
	modinit();

	$(document.body).append($('<button id="testselect">select</button>'));
	$(document.body).append($('<button id="testinsert">insert</button>'));
	$(document.body).append($('<button id="testupdate">update</button>'));
	$(document.body).append($('<button id="testdelete">delete</button>'));
	$(document.body).append($('<button id="testimport">import</button>'));
	$(document.body).append($('<button id="testexport">export</button>'));
	$(document.body).append($('<button id="testsetpass">setpass</button>'));

	$("#testselect").click(function() {
		var tblname = "vcard_person";
		var start   = 0;
		var count   = 5;

		var argobj = {"tblname":tblname, "start":start,"count":count};
		var argstr = JSON.stringify(argobj);

		var arg    = {"argements" : argstr}

		console.log("------------------------------------------------");
		console.log(argstr);


		DoRequest("/api/db/select", arg, function(data) { //success
			console.log(data);
		}, function(stat) { //error
			console.log("Requst /api/db/select... Error:" + stat);
		}, function(stat) {
			console.log("Requst /api/db/select... Complete!");
		});
	});
	$("#testinsert").click(function() {
		var tblname = "vcard_person";

		var vuuid   = Math.random();
		var puuid   = Math.random();
		var newval  = {"vcard_uuid":vuuid.toString(), "person_uuid":puuid.toString()};

		var argobj = {"tblname":tblname, "newval":newval};
		var argstr = JSON.stringify(argobj);

		//alert(argstr);
		console.log("------------------------------------------------");
		console.log(argstr);


		var arg    = {"argements" : argstr}

		DoRequest("/api/db/insert", arg, function(data) { //success
			console.log(data);
		}, function(stat) { //error
			console.log("Requst /api/db/insert... Error:" + stat);
		}, function(stat) {
			console.log("Requst /api/db/insert... Complete!");
		});
	});
	$("#testupdate").click(function() {
		var tblname = "vcard_person";

		var vuuid   = "0.21145866070747066";
		var puuid   = "0.30074283658240275";
		var oldval  = {"vcard_uuid":vuuid.toString(), "person_uuid":puuid.toString()};

		var vuuid   = Math.random();
		var puuid   = Math.random();
		var newval  = {"vcard_uuid":vuuid.toString(), "person_uuid":puuid.toString()};

		var argobj = {"tblname":tblname, "newval":newval, "match":oldval};
		var argstr = JSON.stringify(argobj);

		var arg    = {"argements" : argstr}

		console.log("------------------------------------------------");
		console.log(argstr);


		DoRequest("/api/db/update", arg, function(data) { //success
			console.log(data);
		}, function(stat) { //error
			console.log("Requst /api/db/update... Error:" + stat);
		}, function(stat) {
			console.log("Requst /api/db/update... Complete!");
		});
	});
	$("#testdelete").click(function() {
		var tblname = "vcard_person";

		var vuuid   = "0.1335953555108842";
		var oldval  = {"vcard_uuid":vuuid.toString()};

		var argobj = {"tblname":tblname, "match":oldval};
		var argstr = JSON.stringify(argobj);

		var arg    = {"argements" : argstr}

		console.log("------------------------------------------------");
		console.log(argstr);

		DoRequest("/api/db/delete", arg, function(data) { //success
			console.log(data);
		}, function(stat) { //error
			console.log("Requst /api/db/delete... Error:" + stat);
		}, function(stat) {
			console.log("Requst /api/db/delete... Complete!");
		});
	});
	$("#testimport").click(function() {

		var dburl  = "http://dusun.com/xxx.db";
		var argobj = {"dburl":dburl};
		var argstr = JSON.stringify(argobj);

		var arg    = {"argements" : argstr}


		console.log("------------------------------------------------");
		console.log(argstr);

		DoRequest("/api/db/import", arg, function(data) { //success
			console.log(data);
		}, function(stat) { //error
			console.log("Requst /api/db/import... Error:" + stat);
		}, function(stat) {
			console.log("Requst /api/db/import... Complete!");
		});
	});
	$("#testexport").click(function() {
		var argobj = {};
		var argstr = JSON.stringify(argobj);

		var arg    = {"argements" : argstr}

		console.log("------------------------------------------------");
		console.log(argstr);

		DoRequest("/api/db/export", arg, function(data) { //success
			console.log(data);
		}, function(stat) { //error
			console.log("Requst /api/db/export... Error:" + stat);
		}, function(stat) {
			console.log("Requst /api/db/export... Complete!");
		});
	});
	$("#testsetpass").click(function() {
		var oldpass = "1212";
		var newpass = "3434";
		var argobj = {"oldpass":oldpass, "newpass":newpass};
		var argstr = JSON.stringify(argobj);

		var arg    = {"argements" : argstr}

		console.log("------------------------------------------------");
		console.log(argstr);

		DoRequest("/api/ad/setpass", arg, function(data) { //success
			console.log(data);
		}, function(stat) { //error
			console.log("Requst /api/ad/setpass... Error:" + stat);
		}, function(stat) {
			console.log("Requst /api/ad/setpass... Complete!");
		});
	});
}); 
