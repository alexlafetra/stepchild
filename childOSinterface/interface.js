'use strict';

let stepchildPort;
let connectedToStepchild = false;

function setup(){
    stepchildPort = createSerial();
}

function connectToPort(){
    if (!stepchildPort.opened()) {
        stepchildPort.open(9600);

        //updating vars
        connectedToStepchild = true;
        ConsoleText.addLine("Connected!")

        //rerendering components
        conversionRoot.render(downloadButton(DownloadButton));
        consoleButtonRoot.render(connectButton(ConnectButton));
        consoleTextRoot.render(consoleText(ConsoleText));
    } 
    else {
        disconnect();
    }
}
function disconnect(){
    stepchildPort.close();
    connectedToStepchild = false;
    consoleObject.addLine("Disconnected!");

    conversionRoot.render(downloadButton(DownloadButton));
    consoleButtonRoot.render(connectButton(ConnectButton));
    consoleTextRoot.render(consoleText(ConsoleText));
}

class ConnectButton extends React.Component{
    constructor(props) {
        super(props);
        this.state = { connected: false };
    }
    render() {
        if(connectedToStepchild){
            return (connectButton(
                'button',
                {onClick: () => disconnect()},
                'Disconnect')
            );
        }
        return connectButton(
            'button',
            {onClick: () => connectToPort()},
            'Connect to a Stepchild'
        );
    }
}

//function that uploads a file to the stepchild
function uploadToStepchild(){

}

class UploadButton extends React.Component {
  constructor(props) {
    super(props);
  }
  render() {
    return uploadButton(
        'button',
        {},    
        'upload'
    );
  }
}

function downloadFile(){

}

class DownloadButton extends React.Component {
    constructor(props) {
      super(props);
    }
    render() {
        if(connectedToStepchild){
            return downloadButton(
                'button',
                { onClick: () => downloadFile()},
                'download'
            );
        }
        else{
            return downloadButton(
                'p',
                {},
                'connect to a Stepchild'
            );
        }
    }
}

const maxLines = 8;

class ConsoleText extends React.Component{
    constructor(props) {
        super(props);
        this.data = {text: ["Hello! Connect to a Stepchild to begin.\n"]};
    }
    render(){
        return consoleText(
            'p',
            {},
            this.data.text
        );
    }
    addLine(newLine){
        this.data.text.push(newLine+"\n");
        while(this.data.text.length>maxLines){
            this.data.text.shift();
        }
    }
}

const connectButton = React.createElement;
const downloadButton = React.createElement;
const consoleText = React.createElement;
const consoleObject = new ConsoleText();

const consoleButtonContainer = document.getElementById("console_button_container");
const consoleTextContainer = document.getElementById("console_text_container");
const conversionContainer = document.getElementById("conversion_container");
const transmissionContainer = document.getElementById("transmission_container");

const consoleButtonRoot = ReactDOM.createRoot(consoleButtonContainer);
const consoleTextRoot = ReactDOM.createRoot(consoleTextContainer);
const conversionRoot = ReactDOM.createRoot(conversionContainer);
const transmissionRoot = ReactDOM.createRoot(transmissionContainer);

conversionRoot.render(downloadButton(DownloadButton));
// consoleTextRoot.render(consoleText(ConsoleText));
// consoleTextRoot.render(consoleObject);
consoleButtonRoot.render(connectButton(ConnectButton));
// consoleButtonRoot.render(downloadButton(DownloadButton));