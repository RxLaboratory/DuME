
//encapsulate to avoid global variables
(function()
{
    //open project

    var projectFile = new File("Render Presets.aep");
    app.beginSuppressDialogs();
    app.open(projectFile);
    app.endSuppressDialogs(false);

    //get rqitems

    var rqueueItems = app.project.renderQueue.items;

    //save presets
    var item = rqueueItems[1];
    item.saveAsTemplate("DuBest");
    var om = item.outputModules[1];
    om.saveAsTemplate("DuWAV");
    var item = rqueueItems[2];
    item.saveAsTemplate("DuMultiMachine");
    var om = item.outputModules[1];
    om.saveAsTemplate("DuEXR");

    //close and quit
    app.project.close(CloseOptions.DO_NOT_SAVE_CHANGES);
    alert("Render templates correctly installed.\n You can quit After Effects.");
})();