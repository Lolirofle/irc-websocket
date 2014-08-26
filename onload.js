function initWebsocketIrc(){
	document.getElementsByClassName('messageSend')[0].addEventListener('click',function(e){
		e.preventDefault();
		if($('#messageInput').val()){
			//Send to server
			websocket.send($('#messageInput').val());

			//Output to browser
			var now = new Date();
			$('#messages').append(
				$('<div>')
					.append($('<div>',{text: zeroPad(now.getHours(),2)+":"+zeroPad(now.getMinutes(),2)+":"+zeroPad(now.getSeconds(),2)}).addClass('time'))
					.append($('<div>',{text: "Me"}).addClass('title'))
					.append($('<div>',{text: $('#messageInput').val()}).addClass('text'))
					.addClass('message')
			);
			
			//Clear input box
			$('#messageInput').val('');
		}
	});


	document.getElementsByClassName('nickChange')[0].addEventListener('click',function(e){
		e.preventDefault();

		var name = prompt("New nickname","");
		if(name!=null){
			websocket.send("/nick "+name);
		} 
	});
}
