//encapsulate the script in a function to avoid global variables
(function (thisObj) {

    //================
    #include DuAEF.jsxinc
    DuAEF.init("DuME", "0.1.0");

    DuAEF.forumURL = 'https://forum.rainboxlab.org';
    DuAEF.chatURL = 'http://chat.rainboxlab.org';
    DuAEF.bugReportURL = 'https://github.com/Rainbox-dev/DuME/issues/new?assignees=&labels=Bug&template=bug_report.md&title=';
    //DuAEF.featureRequestURL = 'https://github.com/Rainbox-dev/DuAEF/issues/new?template=feature_request.md';
    DuAEF.aboutURL = 'https://rainboxlab.org/tools/dume/';
    DuAEF.scriptAbout = 'The Duduf Media Encoder\nDeveloped by Nicolas Dufresne and Contributors.\n\nLicensed under the GNU General Public License v3.';
    DuAEF.newsServer = 'rainboxprod.net'
    DuAEF.newsArgs = 'wp/?call_custom_simple_rss=1&csrp_show_meta=0&csrp_cat=20';

    DuMEPath = "";

    // ================ FUNCTIONS =============

    //MAIN

    function loadSettings()
    {
        if (app.settings.haveSetting("DuME","DuMEPath")) setDuMEPath( app.settings.getSetting( "DuME","DuMEPath" ));
    }

    function setupTemplates()
    {
        //TODO first check if the presets are already installed

        //TODO (store and) extract the aep from the include

        var previousProject = app.project.file;

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

        //close and reopen previous
        app.project.close(CloseOptions.DO_NOT_SAVE_CHANGES);
        app.open( previousProject );
    }

    function addCompToDuME()
    {
        try {
        //TODO Use sockets in DuME.
        //In the meantime, use command line arguments

        //get comp name (and check if unique, rename if needed)
        var comp = DuAEF.DuAE.Project.getActiveComp();
        if (!comp) return;
        DuAEF.DuAE.Project.setUniqueCompNames();
        var compName = comp.name;
        //get framerate
        //get duration/numFrames
        var framerate = comp.frameRate;
        var numFrames = comp.duration / comp.frameDuration;

        //save project with a new name
        var projectFile = app.project.file;

        var projectFolder = projectFile.parent;
        var dumeFolder = new Folder( projectFolder.absoluteURI + "/DuME aep/");
        if (!dumeFolder.exists) dumeFolder.create();

        var projectName = DuAEF.DuJS.Fs.getBasename( projectFile );

        var currentDate = new Date();
        var dumeProjectFile = new File( dumeFolder.absoluteURI + "/" + projectName + " (" + DuAEF.DuJS.Date.toString(currentDate) + ").aep");
        app.project.save( dumeProjectFile );

        //launch process
        DuMEProcess = new DuProcess( DuMEPath );
        DuMEProcess.start( ["-comp", compName, "-framerate", framerate,  dumeProjectFile.fsName ]);

        app.project.close( CloseOptions.DO_NOT_SAVE_CHANGES );
        app.open( projectFile );

        }catch(e)  {alert(e)};
    }

    function selectDuMEPath()
    {
        var prompt = "";
        if (DuAEF.win) prompt = "Select the DuME.exe file.";
        else prompt = "Select the DuME app";
        var duMEFile = File.openDialog( prompt, "DuME:DuME.exe" );
        if (duMEFile == null) return;

        setDuMEPath( duMEFile.fsName );
    }

    function setDuMEPath( path )
    {
        if (!checkDuMEPath( path )) return;
        DuMEPath = path;
        browseDuMEEdit.setText( path );
        app.settings.saveSetting("DuME","DuMEPath", path );
    }

    function checkDuMEPath( path )
    {
        if (typeof path === 'undefined') path = DuMEPath;

        var DuMEFile = new File( path );
        return DuMEFile.exists;
    }

    //UI EVENTS

    function settingsButton_clicked() {
        mainGroup.visible = false;
        settingsGroup.visible = true;
    }

    function settingsCancelutton_clicked() {
        mainGroup.visible = true;
        settingsGroup.visible = false;
    }

    function addCompButton_clicked() {
        if (!checkDuMEPath()) selectDuMEPath();
        if (!checkDuMEPath()) return;

        //ask to save project
        var ok = confirm("The project must be saved first. Are you sure you want to continue?");
        if (!ok) return;
        app.project.save();

        //setupTemplates();

        addCompToDuME();
    }

    // _______ UI SETUP _______

    #include "dume_required/icons.jsxinc"

    var ui = DuAEF.DuScriptUI.createUI(thisObj);

    ui.contents.orientation = 'stack';
    var mainGroup = DuAEF.DuScriptUI.addGroup(ui.contents, 'column');
    mainGroup.alignment = ['fill', 'top'];
    mainGroup.alignChildren = ['fill', 'fill'];

    var addCompButton = DuAEF.DuScriptUI.addButton(
        mainGroup,
        "Send comp. to DuME",
        DuAEF.DuBinary.toFile(dume),
        "Add the composition as a new input in DuME"
        );

    var settingsButton = DuAEF.DuScriptUI.addButton(
        mainGroup,
        '',
        DuAEF.DuBinary.toFile(w18_settings_l),
        "Change the settings",
        DuAEF.DuBinary.toFile(w18_settings_r)
        );
    settingsButton.alignment = ['right', 'fill'];

    var settingsGroup = DuAEF.DuScriptUI.addGroup(ui.contents, 'column');
    settingsGroup.alignment = ['fill', 'top'];
    settingsGroup.alignChildren = ['fill', 'fill'];
    settingsGroup.visible = false;

    var browseDuMEGroup = DuAEF.DuScriptUI.addGroup(settingsGroup, 'row');
    
    var browseDuMEButton = DuAEF.DuScriptUI.addButton(
        browseDuMEGroup,
        "Browse",
        undefined,
        "Select DuME"
        );
    browseDuMEButton.alignment = ['left','fill'];
    var browseDuMEEdit = DuAEF.DuScriptUI.addEditText(
        browseDuMEGroup,
        "",
        "",
        "",
        "Path to DuME..."
        );

    var settingsValidGroup = DuAEF.DuScriptUI.addGroup(settingsGroup, 'row');

    var settingsCancelutton = DuAEF.DuScriptUI.addButton(
        settingsValidGroup,
        "<- Back",
        undefined
        );


    // Connections
    settingsButton.onClick = settingsButton_clicked;
    settingsCancelutton.onClick = settingsCancelutton_clicked;
    browseDuMEButton.onClick = selectDuMEPath;
    addCompButton.onClick = addCompButton_clicked;

    //Init
    loadSettings();

    //Show UI
    DuAEF.DuScriptUI.showUI(ui);
    DuAEF.enterRunTime();
})(this);