const { app, BrowserWindow } = require("electron");
const path = require("path");

app.whenReady().then(async () => {
  const mainWindow = new BrowserWindow({
    width: 1000,
    height: 680,
    minWidth: 810,
    minHeight: 600,
    show: false,
    maximizable: true,
    webPreferences: {
      sandbox: false,
      nodeIntegration: true,
      nodeIntegrationInWorker: true,
      webSecurity: false,
      contextIsolation: false,
    },
  });

  mainWindow.loadURL("http://localhost:5173");

  mainWindow.on("ready-to-show", () => {
    mainWindow.maximize();
    mainWindow.show();
  });
});
