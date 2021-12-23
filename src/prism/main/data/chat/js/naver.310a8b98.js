(function(e){function t(t){for(var n,i,o=t[0],c=t[1],l=t[2],m=0,d=[];m<o.length;m++)i=o[m],Object.prototype.hasOwnProperty.call(s,i)&&s[i]&&d.push(s[i][0]),s[i]=0;for(n in c)Object.prototype.hasOwnProperty.call(c,n)&&(e[n]=c[n]);h&&h(t);while(d.length)d.shift()();return r.push.apply(r,l||[]),a()}function a(){for(var e,t=0;t<r.length;t++){for(var a=r[t],n=!0,o=1;o<a.length;o++){var c=a[o];0!==s[c]&&(n=!1)}n&&(r.splice(t--,1),e=i(i.s=a[0]))}return e}var n={},s={naver:0},r=[];function i(t){if(n[t])return n[t].exports;var a=n[t]={i:t,l:!1,exports:{}};return e[t].call(a.exports,a,a.exports,i),a.l=!0,a.exports}i.m=e,i.c=n,i.d=function(e,t,a){i.o(e,t)||Object.defineProperty(e,t,{enumerable:!0,get:a})},i.r=function(e){"undefined"!==typeof Symbol&&Symbol.toStringTag&&Object.defineProperty(e,Symbol.toStringTag,{value:"Module"}),Object.defineProperty(e,"__esModule",{value:!0})},i.t=function(e,t){if(1&t&&(e=i(e)),8&t)return e;if(4&t&&"object"===typeof e&&e&&e.__esModule)return e;var a=Object.create(null);if(i.r(a),Object.defineProperty(a,"default",{enumerable:!0,value:e}),2&t&&"string"!=typeof e)for(var n in e)i.d(a,n,function(t){return e[t]}.bind(null,n));return a},i.n=function(e){var t=e&&e.__esModule?function(){return e["default"]}:function(){return e};return i.d(t,"a",t),t},i.o=function(e,t){return Object.prototype.hasOwnProperty.call(e,t)},i.p="";var o=window["webpackJsonp"]=window["webpackJsonp"]||[],c=o.push.bind(o);o.push=t,o=o.slice();for(var l=0;l<o.length;l++)t(o[l]);var h=c;r.push([1,"chunk-vendors","chunk-common"]),a()})({1:function(e,t,a){e.exports=a("8791")},"22d2":function(e,t,a){},8791:function(e,t,a){"use strict";a.r(t);a("e260"),a("e6cf"),a("cca6"),a("a79d");var n=a("2b0e"),s=function(){var e=this,t=e.$createElement,a=e._self._c||t;return a("chats-wrapper",{ref:"chatsWrapper",attrs:{"key-field":"commentNo","time-field":"modTimeGmt","message-field":"contents",chat:e.renderData,"no-live-tips":e.$i18n.noLiveNaver.replace("${platform}",e.channelName),"show-chats":e.showChats,"disable-input":!e.showChats||e.isPlatformClose,"max-input-character":e.maxInputCharactor},on:{"live-init":e.handleLiveInit,"live-end":e.handleLiveEnd,"live-platform-close":e.handlePlatformClose,"handle-pull-up":e.handlePullUp,"handle-cached-exceed":e.handleCachedExceed,"re-broadcast":e.handleReBroadcast,"handle-send":e.handleSend},scopedSlots:e._u([{key:"chat",fn:function(t){var n=t.item;return[a("template-naver",{attrs:{isVlive:e.isVlive,isDev:e.isDev,data:n},on:{"handle-delete":e.handleDelete,"handle-report":e.handleReport,"handle-retry":e.handleRetry,"show-operation-buttons":function(e){n._isShowErrorHandleBtns=!0}}})]}}])})},r=[],i=(a("a4d3"),a("e01a"),a("99af"),a("4de4"),a("7db0"),a("c740"),a("4160"),a("caad"),a("c975"),a("a434"),a("b0c0"),a("d3b7"),a("ac1f"),a("2532"),a("3ca3"),a("841c"),a("159b"),a("ddb0"),a("2b3d"),a("2909")),o=a("5530"),c=a("de36"),l=a("eb9e"),h=a("7ea6"),m=a("2f14"),d=a("2869"),u=a("487d"),f=function(){var e=this,t=e.$createElement,a=e._self._c||t;return a("div",{staticClass:"chat-info-item naver-chat-item",class:{"is-self":e.data._isSelf,"is-manager":e.data._isManager}},[a("div",{staticClass:"chat-content"},[a("div",{staticClass:"title"},[e.isVlive&&e.data.extension&&e.data.extension.bdg?a("div",{staticClass:"icon-badge",class:{"badge-gray":0!==+e.data.extension.t}},[e._v(e._s(e.data.extension.bdg))]):e._e(),a("div",{staticClass:"author-name"},[a("span",[e._v(e._s(e.data.userName))])])]),a("div",{staticClass:"message"},[e.data.stickerId?a("img",{staticClass:"message-emoji",attrs:{src:e.getChatImageUrl,alt:""}}):e._e(),a("span",{domProps:{innerHTML:e._s(e.data.contents)}})]),a("div",{staticClass:"chat-sup"},[a("span",{staticClass:"publish-time"},[e._v(e._s(e.data.timeago))]),e.data._isSelf||e.data._isManager?e._e():[a("i",{staticClass:"separate-dot"}),a("button",{staticClass:"btn-report",on:{click:e.handleReport}},[e._v(e._s(e.$i18n.report))])]],2),e.data._isSelf?a("div",{staticClass:"chat-btns"},[e.data._sendError?[e.data._isShowErrorHandleBtns?a("div",[a("button",{staticClass:"btn-retry",attrs:{"data-title":e.$i18n.tooltip.retry},on:{click:e.handleRetry}}),a("btn-close",{attrs:{"is-small":"","data-title":e.$i18n.tooltip.deleteText},on:{click:function(t){return e.handleDelete(!0)}}})],1):a("button",{staticClass:"btn-send-error",attrs:{"data-title":e.$i18n.tooltip.sendError},on:{click:e.showButtons}})]:a("btn-close",{staticClass:"chat-btn-close",attrs:{"is-small":"","data-title":e.$i18n.tooltip.deleteText},on:{click:e.handleDelete}})],2):e._e()])])},p=[],g=a("4d06"),C={name:"templateNaver",components:{BtnClose:g["a"]},props:{data:{type:Object,default:function(){return{}}},isDev:Boolean,isVlive:Boolean},computed:{getChatImageUrl:function(){return"".concat(this.isDev?c["VLIVE_CHAT_IMAGE_BASE_URL"]:c["VLIVE_CHAT_IMAGE_BASE_URL_PRO"]).concat(this.data.stickerId,".png")}},methods:{handleReport:function(){this.$emit("handle-report",this.data)},handleDelete:function(e){this.$emit("handle-delete",this.data,e)},handleRetry:function(){this.$emit("handle-retry",this.data)},showButtons:function(){this.$emit("show-operation-buttons")}}},v=C,b=(a("a508"),a("2877")),_=Object(b["a"])(v,f,p,!1,null,null,null),O=_.exports,T=a("d068"),E={name:"prismNaver",components:{ChatsWrapper:u["a"],TemplateNaver:O},data:function(){return{channel:"",chatData:[],isLiving:!1,showChats:!1,prismParams:{},commentManager:null,reportedIDs:[],sendErrorList:[],noMorePreviousComment:!1,pageSize:50,pollTime:5e3,showLoading:!1,bufferManager:null,isPlatformClose:!1,isDev:!1}},computed:{isVlive:function(){return this.channel===c["CHANNEL"].VLIVE},renderData:function(){return this.chatData.concat(this.sendErrorList)},maxInputCharactor:function(){return this.isVlive?c["MAX_INPUT_CHARACTER_VLIVE"]:c["MAX_INPUT_CHARACTER_NAVERTV"]},channelName:function(){return this.channel&&c["CHANNEL_NAME"][this.channel]}},methods:{handleLiveInit:function(e){var t=this;if(!this.isLiving&&(this.clearPage(),e&&e.platforms&&(this.prismParams=e.platforms.find((function(e){return e.name===t.channel})),this.isDev=-1!==this.prismParams.GLOBAL_URI.indexOf("dev-"),this.prismParams))){if(Object(T["a"])({message:this.channel+"Chat: Get the initialization data"}),this.prismParams.name===c["CHANNEL"].VLIVE&&!this.prismParams.objectId)return void Object(T["a"])({message:this.channel+"Chat: vlive need objectId but no objectId"},"error");if(this.prismParams.isRehearsal)return;this.isPlatformClose=!!this.prismParams.disable,this.showChats=!0,this.isLiving=!0,this.handleCommentManagerInit(),this.bufferManager=new h["a"](this.chatData,{pollTime:this.pollTime})}},handlePlatformClose:function(e){e.platform===this.channel&&(Object(T["a"])({message:this.channel+"Chat: This chat platform is disabled"}),this.isPlatformClose=!0)},handleLiveEnd:function(){Object(T["a"])({message:this.channel+"Chat: The live broadcast is over"}),this.isLiving=!1},handleCommentManagerInit:function(){var e=this;this.commentManager||(this.commentManager=new d["a"](Object(o["a"])({type:this.channel,pollTime:this.pollTime,pageSize:this.pageSize},this.prismParams)),this.commentManager.on("NEW_COMMENTS",(function(t){switch(t.type){case"last":e.handlePreviousComment(t.list);break;case"next":e.handleImmediateComment(t.list);break}})),this.commentManager.start())},handlePreviousComment:function(e){if(e.length){var t,a=this.filterCommentData(e);if(a.length)(t=this.chatData).push.apply(t,Object(i["a"])(a))}},handleImmediateComment:function(e){if(e.length){var t=this.filterCommentData(e,!0);t.length&&(this.bufferManager.push(t),t.length&&Object(T["a"])({message:this.channel+"Chat: Receive new chat"}))}},filterCommentData:function(e,t){var a=this,n=[],s=e.reverse(),r=new Date;return s.forEach((function(e){if(!a.reportedIDs.includes(e.userIdNo)){e.extension&&(e.extension=Object(m["a"])(decodeURIComponent(e.extension)));var s=a.getUserPermission(e),i=s.isSelf,o=s.isManager;e._isSelf=i,e._isManager=o,e.timeago=t?Object(l["a"])(r,a.$lang):Object(l["a"])(e.modTimeGmt,a.$lang),n.push(e)}})),n},clearPage:function(){this.platformClose=!1,this.showChats=!1,this.chatData=[],this.prismParams={},this.reportedIDs=[],this.sendErrorList=[],this.noMorePreviousComment=!1,this.clearCommentManager(),this.clearBufferManager()},clearCommentManager:function(){this.commentManager&&(this.commentManager.stop(),this.commentManager=null)},clearBufferManager:function(){this.bufferManager&&(this.bufferManager.clearTimer(),this.bufferManager=null)},handleReport:function(e){var t=this,a=this.$i18n,n=a.notice,s=a.confirm,r=s.reportText,o=s.cancelButton,l=a.alert,h=l.confirmButton,m=l.defaultText;this.$confirm({title:n,content:r,confirmText:h,cancelText:o}).then((function(){Object(T["a"])({message:t.channel+"Chat: Report request for comment start"}),t.commentManager.request.repComment(e.commentNo).then((function(){var a;t.reportedIDs.push(e.userIdNo);var n=t.chatData.filter((function(t){return t.userIdNo!==e.userIdNo}));t.chatData.length=0,(a=t.chatData).push.apply(a,Object(i["a"])(n)),c["CHANNEL_ON_ALL_PAGE"].includes(t.channel)&&window.sendToPrism(JSON.stringify({type:"broadcast",data:{receive:"ALL",type:"report",data:e.userIdNo}})),Object(T["a"])({message:t.channel+"Chat: Report request for comment success"})})).catch((function(e){t.$alert({title:n,content:m,confirmText:h}).then((function(){})),Object(T["a"])({message:t.channel+"Chat: Report request for comment error"},"error",e)}))})).catch((function(){}))},handleDelete:function(e,t){var a=this,n=this.$i18n,s=n.notice,r=n.confirm,i=r.deleteText,o=r.deleteConfirmButton,l=r.cancelButton,h=n.alert,m=h.defaultText,d=h.confirmButton;this.$confirm({title:s,content:i,confirmText:o,cancelText:l}).then((function(){t?a.handleDeleteSendErrorItem(e):(Object(T["a"])({message:a.channel+"Chat: delete chat request start"}),a.commentManager.request.delComment(e.commentNo).then((function(){var t=a.chatData.indexOf(e);t>-1&&a.chatData.splice(t,1),c["CHANNEL_ON_ALL_PAGE"].includes(a.channel)&&window.sendToPrism(JSON.stringify({type:"broadcast",data:{receive:"ALL",type:"delete",data:e.commentNo}})),Object(T["a"])({message:a.channel+"Chat: Request to delete comment success"})})).catch((function(e){a.$alert({title:s,content:m,confirmText:d}).then((function(){})),Object(T["a"])({message:a.channel+"Chat: Request to delete comment error",data:e},"error")})))})).catch((function(){}))},handleDeleteSendErrorItem:function(e){var t=this.sendErrorList.findIndex((function(t){return t.commentNo===e.commentNo}));t>-1&&this.sendErrorList.splice(t,1)},handleSend:function(e){var t=this;Object(T["a"])({message:this.channel+"Chat: start send chat"}),this.commentManager.request.sendComment({commentType:"txt",contents:e}).then((function(e){var a;Object(T["a"])({message:t.channel+"Chat: send chat success"}),(a=t.chatData).push.apply(a,Object(i["a"])(t.filterCommentData(e)))})).catch((function(a){Object(T["a"])({message:t.channel+"Chat: send chat error",data:a},"error");var n=t.$i18n,s=n.notice,r=n.alert,i=r.confirmButton,o=r.defaultText,c=r.sensitiveText;"5020"===a.code?t.$alert({title:s,content:c,confirmText:i}):t.$alert({title:s,content:o,confirmText:i}).then((function(){var a=t.prismParams,n=new Date;t.sendErrorList.push({commentNo:Symbol(),modTimeGmt:n,_isSelf:!0,_isManager:!1,timeago:Object(l["a"])(n,t.$lang),extension:"",userName:a.username,userProfileImage:a.userProfileImage,contents:e,_sendError:!0,_isShowErrorHandleBtns:!1,_isSending:!1})}))}))},handleRetry:function(e){var t=this;e._isSending||(e._isSending=!0,Object(T["a"])({message:this.channel+"Chat: start resend chat"}),this.commentManager.request.sendComment({commentType:"txt",contents:e.contents}).then((function(a){var n;Object(T["a"])({message:t.channel+"Chat: resend chat success"}),t.handleDeleteSendErrorItem(e),(n=t.chatData).push.apply(n,Object(i["a"])(t.filterCommentData(a)))})).catch((function(a){Object(T["a"])({message:t.channel+"Chat: resend chat error",data:a},"error");var n=t.$i18n,s=n.notice,r=n.alert,i=r.confirmButton,o=r.defaultText;t.$alert({title:s,content:o,confirmText:i}).then((function(){})),e._isShowErrorHandleBtns=!1})).finally((function(){e._isSending=!1})))},getUserPermission:function(e){var t=!1,a=!1;switch(this.channel){case c["CHANNEL"].VLIVE:t=e.mine,a=!t&&e.manager;break;case c["CHANNEL"].NAVERTV:t=this.prismParams.userId===e.userIdNo,a=!t&&e.manager;break}return{isSelf:t,isManager:a}},handlePullUp:function(){var e=this;if(!this.noMorePreviousComment&&!this.showLoading){this.showLoading=!0;var t=this.chatData[0];Object(T["a"])({message:this.channel+"Chat: getOlderComment chat start"}),this.commentManager.request.getOlderComment(t.commentNo,this.pageSize).then((function(t){if(e.showLoading=!1,t.length){var a,n=e.filterCommentData(t);if(n.length)(a=e.chatData).unshift.apply(a,Object(i["a"])(n)),e.$nextTick((function(){e.$refs.chatsWrapper&&e.$refs.chatsWrapper.$refs.scroller.scrollToItem(n.length-1)}));Object(T["a"])({message:e.channel+"Chat: getOlderComment chat success"})}else e.noMorePreviousComment=!0})).catch((function(t){Object(T["a"])({message:e.channel+"Chat: getOlderComment chat error",data:t},"error"),e.showLoading=!1}))}},handleCachedExceed:function(){var e=this.chatData.length;if(!(e<=c["MAX_CACHED_CHAT_COUNT"])){var t=e-c["MAX_CACHED_CHAT_COUNT"]+.1*c["MAX_CACHED_CHAT_COUNT"];this.chatData.splice(0,t),this.noMorePreviousComment=!1}},handleReBroadcast:function(e){if(e=Object(m["a"])(e),e.receive===this.channel)switch(e.type){case"delete":var t=this.chatData.findIndex((function(t){return t.commentNo===e.data}));-1!==t&&this.chatData.splice(t,1);break}}},created:function(){var e=new URLSearchParams(document.location.search).get("platform")||"";if(this.channel=e.toLocaleUpperCase(),![c["CHANNEL"].NAVERTV,c["CHANNEL"].VLIVE].includes(this.channel))throw new Error("Please take the correct platform name on url")},mounted:function(){Object(T["a"])({message:this.channel+"Chat: The chat page loads successfully"})},beforeDestroy:function(){Object(T["a"])({message:this.channel+"Chat: The chat page is closed"}),this.clearCommentManager(),this.clearBufferManager()}},N=E,M=Object(b["a"])(N,s,r,!1,null,null,null),L=M.exports;a("21af");a("58d1").default(n["a"]),a("8e7d").default(n["a"]),n["a"].config.productionTip=!1,new n["a"]({render:function(e){return e(L)}}).$mount("#app")},a508:function(e,t,a){"use strict";var n=a("22d2"),s=a.n(n);s.a}});