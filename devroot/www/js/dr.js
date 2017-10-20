$(function(){ 
		var content; 
		//$("#content tr:odd").css("background-color","#D2B48C"); 
		//$("#content tr:even").css("background-color","#C0C0C0"); 
		$("#content .disarea .table td").not(".op").click(function(){ 
			var clickObj = $(this); 
			content = clickObj.html(); 
			changeToEdit(clickObj); 
		}); 

		

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

		$("#prev").click(function() {
			var clickObj = $(this);
			var btntxt = clickObj.html();
			alert(btntxt);
		});

		$("#next").click(function() {
			var clickObj = $(this);
			var btntxt = clickObj.html();
			alert(btntxt);
		});

		$("#add").click(function() {
			var clickObj = $(this);
			var btntxt = clickObj.html();
			alert(btntxt);
		});

		$("#del").click(function() {
			var clickObj = $(this);
			var btntxt = clickObj.html();
			alert(btntxt);
		});

		$("#mod").click(function() {
			var clickObj = $(this);
			var btntxt = clickObj.html();
			alert(btntxt);
		});






}); 
