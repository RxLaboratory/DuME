//encapsulate the script in a function to avoid global variables
(function (thisObj) {

    //================
    #include DuAEF.jsxinc
    #include "dume_required/icons.jsxinc"

    DuAEF.init("DuME", "0.1.5");

    DuAEF.scriptIcon = DuAEF.DuBinary.toFile(w18_rx_l);
    DuAEF.scriptIconOver = DuAEF.DuBinary.toFile(w18_rx_r);
    DuAEF.forumURL = 'https://forum.rainboxlab.org';
    DuAEF.chatURL = 'http://chat.rainboxlab.org';
    DuAEF.bugReportURL = 'https://github.com/Rainbox-dev/DuME/issues/new?assignees=&labels=Bug&template=bug_report.md&title=';
    //DuAEF.featureRequestURL = 'https://github.com/Rainbox-dev/DuAEF/issues/new?template=feature_request.md';
    DuAEF.aboutURL = 'https://rainboxlab.org/tools/dume/';
    DuAEF.docURL = 'http://dume-docs.rainboxlab.org';
    DuAEF.scriptAbout = 'The Duduf Media Encoder\nDeveloped by Nicolas Dufresne and Contributors.\n\nLicensed under the GNU General Public License v3.';
    DuAEF.newsServer = 'rainboxprod.net'
    DuAEF.newsArgs = 'wp/?call_custom_simple_rss=1&csrp_show_meta=0&csrp_cat=20';

    var settings = new DuSettings();

    // ================ FUNCTIONS =============

    //MAIN

    function loadSettings()
    {
        //defaults
        settings.data.dumePath = def(settings.data.dumePath, "");
        settings.data.autoStart = def(settings.data.autoStart, true);
        settings.data.autoQuit = def(settings.data.autoQuit, false);
        settings.data.defaultPreset = def(settings.data.defaultPreset, "");

        setDuMEPath( settings.data.dumePath );
        autoStartButton.setChecked( settings.data.autoStart );
        autoQuitButton.enabled = autoStartButton.checked;
        autoQuitButton.setChecked( settings.data.autoQuit );
    }

    function setupTemplates()
    {
        // first check if the presets are already installed
        var itemDuBest = DuAEF.DuAE.RQueue.hasRenderSettingsTemplate("DuBest");
        var itemDuMultiMachine = DuAEF.DuAE.RQueue.hasRenderSettingsTemplate("DuMultiMachine");
        var omDuWAV = DuAEF.DuAE.RQueue.hasOutputModuleTemplate("DuWAV");
        var omDuEXR = DuAEF.DuAE.RQueue.hasOutputModuleTemplate("DuEXR");
        if (itemDuBest && itemDuMultiMachine && omDuWAV && omDuEXR) return;

        //The aep containing the presets
        #include "dume_required/renderPresets.aep.jsxinc"

        var projectFile = DuAEF.DuBinary.toFile(renderPresets);
        //import
        var io = new ImportOptions(projectFile);
        io.importAs = ImportAsType.PROJECT;
        var tempItem = app.project.importFile(io);

        //get rqitems
        var rqueueItems = app.project.renderQueue.items;

        //save presets
        var item = rqueueItems[ rqueueItems.length - 1 ];
        item.saveAsTemplate("DuBest");
        var om = item.outputModules[1];
        om.saveAsTemplate("DuWAV");
        var item = rqueueItems[ rqueueItems.length ];
        item.saveAsTemplate("DuMultiMachine");
        var om = item.outputModules[1];
        om.saveAsTemplate("DuEXR");

        //remove items
        tempItem.remove();
    }

    function addCompToDuME()
    {
        setupTemplates();
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

        var outputFile = new File( projectFolder.fsName + "/" + projectName )

        //launch process
        DuMEProcess = new DuProcess( settings.data.dumePath );
        var args = ["-comp", compName, "-framerate", framerate,  dumeProjectFile.fsName, "-output", outputFile.fsName, "-preset", presetsButton.text, "-hideconsole"  ];
        if (settings.data.autoStart)
        {
            args.push("-autostart");
            if (settings.data.autoQuit) args.push("-autoquit");
        }
        //TODO maybe add a preset selector
        DuMEProcess.start( args );


        app.project.close( CloseOptions.DO_NOT_SAVE_CHANGES );
        app.open( projectFile );
    }

    function selectDuMEPath()
    {
        var prompt = "";
        if (DuAEF.win) prompt = "Select the DuME.exe file.";
        else prompt = "Select the DuME app";
        var duMEFile = File.openDialog( prompt, "DuME:DuME.exe" );
        if (duMEFile == null) return;

        var path = duMEFile.fsName;

        setDuMEPath( path );
    }

    function setDuMEPath( path )
    {
        path = def(path, settings.data.dumePath);
        var binPath = path;
        // on mac, the actual bin is buried in the app folder
        if (DuAEF.mac) binPath += "/Contents/MacOS/DuME";
        if (!checkDuMEPath( binPath )) return;
        // load DuME version and presets
        
        var output = DuAEF.DuProcess.run(binPath, ["-h:presets"]);

        // process output
        output = output.split("\n");
        var dumeVersion = "";
        var presets = [];
        var inPresets = false;
        for (var i = 0, n = output.length; i < n; i++)
        {
            var l = DuAEF.DuJS.String.trim(output[i]);

            if (inPresets && l != "") presets.push(l);
            if (l.indexOf("version") == 0) dumeVersion = l.substring(8);
            if (l == "Default Preset:" && settings.data.defaultPreset == "") settings.data.defaultPreset = DuAEF.DuJS.String.trim(output[i+1]);
            if (l == "Available presets:" ) inPresets = true;
            
        }

        //check version
        if (dumeVersion != DuAEF.scriptVersion.fullVersion)
        {
            alert( "Warning! The version of DuME is not the same as the version of the After Effects script." +
                "\nSome features may not work properly." + 
                "\nDuME version is " + dumeVersion +
                "\nThe script version is " + DuAEF.scriptVersion.fullVersion
            );
        }

        //populate presets
        presetsButton.removeAll();
        for (var i = 0, n = presets.length; i < n; i++)
        {
            presetsButton.addButton(presets[i]);
        }
        presetsButton.setCurrentText( settings.data.defaultPreset );
        
        browseDuMEEdit.setText( path );
        settings.data.dumePath = path;
        settings.save();
    }

    function checkDuMEPath( pathToBin )
    {
        pathToBin = def(pathToBin, settings.data.dumePath);
        var DuMEFile = new File( pathToBin );
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

    function autoStartButton_clicked() {
        autoQuitButton.enabled = autoStartButton.checked;
        settings.data.autoStart = autoStartButton.checked;
        settings.save();
    }

    function autoQuitButton_clicked() {
        settings.data.autoQuit = autoQuitButton.checked;
        settings.save();
    }

    function presetsButton_changed() {
        settings.data.defaultPreset =  presetsButton.text;
        settings.save();
    }

    // _______ UI SETUP _______

    var ui = DuAEF.DuScriptUI.createMainPanel(thisObj, true, true);

    var addCompButton = DuAEF.DuScriptUI.addButton(
        ui.mainGroup,
        "Send comp. to DuME",
        DuAEF.DuBinary.toFile(dume),
        "Add the composition as a new input in DuME",
        undefined,
        true
    );

    var presetsButton = DuAEF.DuScriptUI.addSelector( addCompButton.optionsPanel, true );
    var autoStartButton = DuAEF.DuScriptUI.addCheckBox( addCompButton.optionsPanel, "Auto-start rendering process");
    autoStartButton.setChecked(true);
    var autoQuitButton = DuAEF.DuScriptUI.addCheckBox(  addCompButton.optionsPanel, "Auto-quit DuME after rendering");

    var browseDuMEGroup = DuAEF.DuScriptUI.addGroup( ui.settingsGroup, 'row');
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
    
    // Connections
    browseDuMEButton.onClick = selectDuMEPath;
    addCompButton.onClick = addCompButton_clicked;
    autoStartButton.onClick = autoStartButton_clicked;
    autoQuitButton.onClick = autoQuitButton_clicked;
    presetsButton.onChange = presetsButton_changed;
    presetsButton.onRefresh = setDuMEPath;

    //Init
    loadSettings();

    //Show UI
    DuAEF.DuScriptUI.showUI(ui);
    DuAEF.enterRunTime();
})(this);