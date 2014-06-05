
var listener = document.getElementById('listener');
listener.addEventListener('load', moduleDidLoad, true);
listener.addEventListener('message', handleMessage, true);
        
var connectButton = document.getElementById('connectButton');
connectButton.addEventListener('click', clikedConnect, true);
        
// Add event listeners once the DOM has fully loaded by listening for the
// `DOMContentLoaded` event on the document, and adding your listeners to
// specific elements when it triggers.
document.body.addEventListener('load', pageDidLoad );