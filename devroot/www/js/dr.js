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

			if (btntxt == "m") {
				$(clickObj).html("M?");
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
				$(clickObj).html("m");
				$(clickObj).parent().parent().children("td").not(".op").attr('onclick', '').unbind('click');
			}
		});



	}

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


}); 
