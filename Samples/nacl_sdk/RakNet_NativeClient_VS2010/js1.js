
// Check for the various File API support.
if (window.File && window.FileReader && window.FileList && window.Blob) {
  // Great success! All the File APIs are supported.
} else {
  alert('The File APIs are not fully supported in this browser.');
}

helloWorldModule = null;  // Global application object.
statusText = 'NO-STATUS';

// Indicate success when the NaCl module has loaded.
function moduleDidLoad() {
  helloWorldModule = document.getElementById('udp_example');
  updateStatus('moduleDidLoad SUCCESS');
}

// Handle a message coming from the NaCl module.
function handleMessage(message_event)
{
		console.log("Got handleMessage");
    console.log(message_event.data);
}

// If the page loads before the Native Client module loads, then set the
// status message indicating that the module is still loading.  Otherwise,
// do not change the status message.
function pageDidLoad() {
if (helloWorldModule == null) {
  updateStatus('LOADING...');
} else {
  // It's possible that the Native Client module onload event fired
  // before the page's onload event.  In this case, the status message
  // will reflect 'SUCCESS', but won't be displayed.  This call will
  // display the current message.
  updateStatus();
}
}

// Set the global status message.  If the element with id 'statusField'
// exists, then set its HTML to the status message as well.
// opt_message The message test.  If this is null or undefined, then
//     attempt to set the element with id 'statusField' to the value of
//     |statusText|.
function updateStatus(opt_message) {
  if (opt_message)
    statusText = opt_message;
  var statusField = document.getElementById('statusField');
  if (statusField) {
    statusField.innerHTML = statusText;
  }
}

// Indicate success when the NaCl module has loaded.
function clikedConnect() {
  helloWorldModule = document.getElementById('udp_example');
  if (helloWorldModule == null) {
  	updateStatus('helloWorldModule == null');
  }
  else
  {
  	updateStatus('Calling postMessage');
		helloWorldModule.postMessage('connect');
	}
}
