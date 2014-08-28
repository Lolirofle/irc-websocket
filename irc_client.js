var IrcMessage_Prefix={
	UNDETERMINED: 1,
	NONE        : 2,
	USER        : 3,
	SERVER      : 4
};

var IrcMessage_CommandType={
	JOIN   : "JOIN",
	PART   : "PART",
	PING   : "PING",
	PONG   : "PONG",
	NICK   : "NICK",
	KICK   : "KICK",
	QUIT   : "QUIT",
	MODE   : "MODE",
	KILL   : "KILL",
	AWAY   : "AWAY",
	TOPIC  : "TOPIC",
	SQUIT  : "SQUIT",
	ERROR  : "ERROR",
	NOTICE : "NOTICE",
	PRIVMSG: "PRIVMSG",
	RPL_WELCOME         : 001,
	RPL_YOURHOST        : 002,
	RPL_CREATED         : 003,
	RPL_MYINFO          : 004,
	RPL_BOUNCE          : 005,
	RPL_TRACELINK       : 200,
	RPL_TRACECONNECTING : 201,
	RPL_TRACEHANDSHAKE  : 202,
	RPL_TRACEUNKNOWN    : 203,
	RPL_TRACEOPERATOR   : 204,
	RPL_TRACEUSER       : 205,
	RPL_TRACESERVER     : 206,
	RPL_TRACENEWTYPE    : 208,
	RPL_STATSLINKINFO   : 211,
	RPL_STATSCOMMANDS   : 212,
	RPL_STATSCLINE      : 213,
	RPL_STATSNLINE      : 214,
	RPL_STATSILINE      : 215,
	RPL_STATSKLINE      : 216,
	RPL_STATSYLINE      : 218,
	RPL_ENDOFSTATS      : 219,
	RPL_UMODEIS         : 221,
	RPL_STATSLLINE      : 241,
	RPL_STATSUPTIME     : 242,
	RPL_STATSOLINE      : 243,
	RPL_STATSHLINE      : 244,
	RPL_LUSERCLIENT     : 251,
	RPL_LUSEROP         : 252,
	RPL_LUSERUNKNOWN    : 253,
	RPL_LUSERCHANNELS   : 254,
	RPL_LUSERME         : 255,
	RPL_ADMINME         : 256,
	RPL_ADMINLOC1       : 257,
	RPL_ADMINLOC2       : 258,
	RPL_ADMINEMAIL      : 259,
	RPL_TRACELOG        : 261,
	RPL_NONE            : 300,
	RPL_AWAY            : 301,
	RPL_USERHOST        : 302,
	RPL_ISON            : 303,
	RPL_UNAWAY          : 305,
	RPL_NOWAWAY         : 306,
	RPL_WHOISUSER       : 311,
	RPL_WHOISSERVER     : 312,
	RPL_WHOISOPERATOR   : 313,
	RPL_WHOWASUSER      : 314,
	RPL_ENDOFWHO        : 315,
	RPL_WHOISIDLE       : 317,
	RPL_ENDOFWHOIS      : 318,
	RPL_WHOISCHANNELS   : 319,
	RPL_LISTSTART       : 321,
	RPL_LIST            : 322,
	RPL_LISTEND         : 323,
	RPL_CHANNELMODEIS   : 324,
	RPL_NOTOPIC         : 331,
	RPL_TOPIC           : 332,
	RPL_INVITING        : 341,
	RPL_SUMMONING       : 342,
	RPL_VERSION         : 351,
	RPL_WHOREPLY        : 352,
	RPL_NAMREPLY        : 353,
	RPL_ENDOFNAMES      : 366,
	RPL_LINKS           : 364,
	RPL_ENDOFLINKS      : 365,
	RPL_BANLIST         : 367,
	RPL_ENDOFBANLIST    : 368,
	RPL_ENDOFWHOWAS     : 369,
	RPL_INFO            : 371,
	RPL_ENDOFINFO       : 374,
	RPL_MOTDSTART       : 375,
	RPL_MOTD            : 372,
	RPL_ENDOFMOTD       : 376,
	RPL_YOUREOPER       : 381,
	RPL_REHASHING       : 382,
	RPL_TIME            : 391,
	RPL_USERSSTART      : 392,
	RPL_USERS           : 393,
	RPL_ENDOFUSERS      : 394,
	RPL_NOUSERS         : 395,
	ERR_NOSUCHNICK      : 401,
	ERR_NOSUCHSERVER    : 402,
	ERR_NOSUCHCHANNEL   : 403,
	ERR_CANNOTSENDTOCHAN: 404,
	ERR_TOOMANYCHANNELS : 405,
	ERR_WASNOSUCHNICK   : 406,
	ERR_TOOMANYTARGETS  : 407,
	ERR_NOORIGIN        : 409,
	ERR_NORECIPIENT     : 411,
	ERR_NOTEXTTOSEND    : 412,
	ERR_NOTOPLEVEL      : 413,
	ERR_WILDTOPLEVEL    : 414,
	ERR_UNKNOWNCOMMAND  : 421,
	ERR_NOMOTD          : 422,
	ERR_NOADMININFO     : 423,
	ERR_FILEERROR       : 424,
	ERR_NONICKNAMEGIVEN : 431,
	ERR_ERRONEUSNICKNAME: 432,
	ERR_NICKNAMEINUSE   : 433,
	ERR_NICKCOLLISION   : 436,
	ERR_USERNOTINCHANNEL: 441,
	ERR_NOTONCHANNEL    : 442,
	ERR_USERONCHANNEL   : 443,
	ERR_NOLOGIN         : 444,
	ERR_SUMMONDISABLED  : 445,
	ERR_USERSDISABLED   : 446,
	ERR_NOTREGISTERED   : 451,
	ERR_NEEDMOREPARAMS  : 461,
	ERR_ALREADYREGISTRED: 462,
	ERR_NOPERMFORHOST   : 463,
	ERR_PASSWDMISMATCH  : 464,
	ERR_YOUREBANNEDCREEP: 465,
	ERR_KEYSET          : 467,
	ERR_CHANNELISFULL   : 471,
	ERR_UNKNOWNMODE     : 472,
	ERR_INVITEONLYCHAN  : 473,
	ERR_BANNEDFROMCHAN  : 474,
	ERR_BADCHANNELKEY   : 475,
	ERR_NOPRIVILEGES    : 481,
	ERR_CHANOPRIVSNEEDED: 482,
	ERR_CANTKILLSERVER  : 483,
	ERR_NOOPERHOST      : 491,
	ERR_UMODEUNKNOWNFLAG: 501,
	ERR_USERSDONTMATCH  : 502,
};

function irc_prepareCommandParam_parseFunc(out){
	switch(out.command.type){
		case IrcMessage_CommandType.JOIN:{
			out.command.join = {
				channels: [],
				keys    : null,
			};
			return function(out,param_count,param){
				out.command.join.channels.push(param);
				return true;
			};
		}
		case IrcMessage_CommandType.PART:{
			out.command.part = {
				channels: [],
				message : null,
			};
			commandParam_parseFunc = function(out,param_count,param){
				out.command.part.channels.push(param);
				return true;
			};
		}
		case IrcMessage_CommandType.PING:{
			out.command.ping = {
				from: null,
				to  : null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.ping.from = param;
						break;
					case 1:
						out.command.ping.to = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}
		case IrcMessage_CommandType.PONG:{
			out.command.pong = {
				from: null,
				to  : null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.pong.from = param;
						break;
					case 1:
						out.command.pong.to = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}
		case IrcMessage_CommandType.NICK:{
			out.command.nick = {
				name: null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.nick.name = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}
		case IrcMessage_CommandType.KICK:{
			out.command.kick = {
				channels: null,
				users   : null,
			};
			return function(out,param_count,param){
				switch(param_count){
						case 0:
							//out.command.kick.channels = param;
							break;
						case 1:
							//out.command.kick.channels = param;
							break;
						case 2:
							out.command.kick.comment = param;
							break;
						default:
							return false;
					}
					return true;
			};
		}
		case IrcMessage_CommandType.QUIT:{
			out.command.quit = {
				message: null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.quit.message = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}
		case IrcMessage_CommandType.MODE:{
			out.command.mode = {
				nickname : null,
				operation: false,
				mode     : '\0',
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.mode.nickname = param;
						break;
					case 1:
						if(param.length<2)
							return false;
						switch(param[0]){
							case '+':
								out.command.mode.operation = true;
								break;
							case '-':
								out.command.mode.operation = false;
								break;
							default:
								return false;
						}
						out.command.mode.mode = param[1];
						break;
					default:
						return false;
				}
				return true;
			};
		}
		case IrcMessage_CommandType.KILL:{
			out.command.kill = {
				nickname: null,
				comment : null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.kill.nickname = param;
						break;
					case 1:
						out.command.kill.comment = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}
		case IrcMessage_CommandType.AWAY:{
			out.command.away = {
				text: null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.away.text = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}

		case IrcMessage_CommandType.TOPIC:{
			out.command.topic = {
				channel: null,
				text   : null,
			};
			return function(out,param_count,param){
				switch(param_count){
						case 0:
							out.command.topic.channel = param;
							break;
						case 1:
							out.command.topic.text = param;
							break;
						default:
							return false;
					}
					return true;
			};
		}
		case IrcMessage_CommandType.SQUIT:{
			out.command.squit = {
				server : null,
				comment: null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.squit.server = param;
						break;
					case 1:
						out.command.squit.comment = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}
		case IrcMessage_CommandType.ERROR:{
			out.command.error = {
				message: "",
			};
			return function(out,param_count,param){
				out.command.error.message+=param;
				return true;
			};
		}

		case IrcMessage_CommandType.NOTICE:{
			out.command.notice = {
				target: null,
				text  : null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.notice.target = param;
						break;
					case 1:
						out.command.notice.text = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}

		case IrcMessage_CommandType.PRIVMSG:{
			out.command.privmsg = {
				target: null,
				text  : null,
			};
			return function(out,param_count,param){
				switch(param_count){
					case 0:
						out.command.privmsg.target = param;
						break;
					case 1:
						out.command.privmsg.text = param;
						break;
					default:
						return false;
				}
				return true;
			};
		}

		default:{
			out.command.unknown = {
				text: [],
			};
			return function(out,param_count,param){
				out.command.unknown.text[param_count] = param;
				return true;
			};
		}
	}
}

function irc_parseMessage(message,onMessageFunc){
	var i = 0;
	var i_begin = i;

	//Prepare termination variable that should be toggled when at end of message
	var IrcParse_Terminate = {
		NOT : 0,
		EOF : 1,
		CRLF: 2,
	};
	var terminate = IrcParse_Terminate.NOT;

	var out = null;

	function checkEnd(){
		if(i>=message.length)
			terminate = IrcParse_Terminate.EOF;
		else if(message.charAt(i) === '\r' && message.charAt(i+1) === '\n')
			terminate = IrcParse_Terminate.CRLF;
		return terminate;
	}

	function end(){
		switch(terminate){
			case IrcParse_Terminate.EOF:
				if(out!==null)
					onMessageFunc(out);
				return;
			case IrcParse_Terminate.CRLF:
				if(out!==null)
					onMessageFunc(out);
				return irc_parseMessage(message.substr(i+2),onMessageFunc);
			default:
				throw "Called irc_parseMessage.end() when not at end";
		}
	}

	//If at end
	if(checkEnd())
		return end();

	out = {
		prefix:{
			type: IrcMessage_Prefix.UNDETERMINED
		},
		command:{},
	};

	//If the message has a prefix
	if(message.charAt(i)===':'){
		i_begin = ++i;
		if(checkEnd())
			return end();

		//Prefix
		for(var continueLoop=true;continueLoop;){
			//If the prefix type hasn't been determined to be in any format and a '!' or '@' is found
			switch(message.charAt(i)){
				case ' ':
					continueLoop = false;

					//If the prefix hasn't been determined to be in any format and it is the end
					if(out.prefix.type === IrcMessage_Prefix.UNDETERMINED){
						//It is the server name (The string from i_begin to i)
						out.prefix.type = IrcMessage_Prefix.SERVER;
						out.prefix.name = message.substr(i_begin,i-i_begin);
					}
					i_begin = ++i;
					break;
				case '@':
				case '!':
					if(out.prefix.type === IrcMessage_Prefix.UNDETERMINED){
						//It is a nickname parameter (The string from i_begin to i)
						out.prefix.nickname = message.substr(i_begin,i-i_begin);
						out.prefix.type = IrcMessage_Prefix.USER;
					}
					//If the prefix type has been determined to be in the user format
					else if(out.prefix.type === IrcMessage_Prefix.USER){
						//Lookup the beginning and determine by that
						switch(i_begin[-1]){
							case '@':
								out.prefix.host = message.substr(i_begin,i-i_begin);
								break;
							case '!':
								out.prefix.username = message.substr(i_begin,i-i_begin);
								break;
							default:
								break;
						}
					}
					i_begin = ++i;
					break;
				default:
					++i;
					break;
			}
			if(checkEnd())
				return end();
		}
	}else
		out.prefix.type = IrcMessage_Prefix.NONE;

	//Command
	var commandParam_parseFunc = null;
	try{while(true){
		//If end of command
		if(message.charAt(i) === ' ')
			break;
		++i;
		if(checkEnd())
			return end();
	}}finally{
		out.command.type = message.substr(i_begin,i-i_begin);
		commandParam_parseFunc = irc_prepareCommandParam_parseFunc(out);

		i_begin = ++i;
	}

	if(checkEnd())
		return end();

	//Params
	if(commandParam_parseFunc){
		var trailing = false;
		var param_count=0;

		StringLoop: while(true){
			if(!trailing){
				switch(message.charAt(i)){
					//Param trailing
					case ':':
						trailing = true;
					
					//End of param
					case ' ':
						if(i_begin!=i){
							//Process the argument
							var param = message.substr(i_begin,i-i_begin);
							if(!commandParam_parseFunc(out,param_count,param))
								throw "Error: Parse error on message command argument "+param_count+": \""+param+"\"";

							//Check if param_count<=14 because the standards says that it is the max amount of parameters
							if(++param_count>=14)
								break StringLoop;
						}
						i_begin = ++i;
						break;
					default:
						++i;
						break;
				}
			}else
				++i;

			if(checkEnd())
				break;
		}

		//Process the argument
		var param = message.substr(i_begin,i-i_begin);
		if(!commandParam_parseFunc(out,param_count,param))
			throw "Error: Parse error on message command argument "+param_count+": \""+param+"\"";
	}

	//Ignore the rest
	while(!checkEnd())
		++i;
	return end();
}
