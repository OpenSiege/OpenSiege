-- **************************** 
-- Siege Max Startup
-- ****************************

-- Verify the DSiegeUtils plugin and DSiege installation
-- by requesting that the Naming Key be loaded

global SIEGEMAX_TOOLKIT_MAJOR_VERSION = 2
global SIEGEMAX_TOOLKIT_MINOR_VERSION = 21
global SIEGEMAX_TOOLKIT_VERSION_STRING = "0002.0021"
 
fn ProductIs3dsMax = 
(
	try
	(
		return (maxOps.productAppID == #max)
	)
	catch
	(
		return  (getfiles ((getdir #maxroot)+"3dsmax.exe")).count == 1
	)		
)
	


dsFatalError = false
try 
(
	if not dsLoadNamingKey() then 
	(
		local msg = "Unable to load the Naming Key\n\n"
		msg += "Files seem to be missing!\n\n"
		msg += "Please check to make sure that both Dungeon Siege\n"
		msg += "and the Dungeon Siege Tool Kit (DSTK) are properly\n"
		msg += "installed on this system\n\n"
		msg += "You will not be able to preview and export properly\n"
		msg += "without a valid Naming Key"
		MessageBox msg title:"Siege Max Initialization Error"
	)
)
catch
(
	dsFatalError = true
	local msg = "Unable to access required plugin!\n\n"
	msg += "The DSiegeUtils.dlu plugin was not loaded\n\n"
	msg += "Siege Max will not run without this plugin\n\n"
	msg += "Please check to make sure that the Dungeon\n"
	msg += "Siege Tool Kit (DSTK) is properly\n"
	msg += "installed on this system\n\n"
	MessageBox msg title:"Siege Max Fatal Error"
)

try
(
	shadowdir = "//packmule/p4_shadow"
	installerdir = shadowdir+"/config/version_info"
	flist = getfiles (installerdir+"/SiegeMaxVersion.txt")
	
	if flist.count > 0 then
	(
		fs = openfile flist[1] mode:"rb"
		majorversion = (readdelimitedstring fs ".")
		minorversion = (readdelimitedstring fs ".")
		close fs
				
		if (majorversion as integer) > SIEGEMAX_TOOLKIT_MAJOR_VERSION then
		(
			local msg = "Your SiegeMax tools are out SERIOUSLY out of date!\n\n"
			msg += "You must update before you can use the SiegeMax Tools\n\n"
			msg += ("Your version is:\t\t"+SIEGEMAX_TOOLKIT_VERSION_STRING + "\n")
			msg += ("The current version is:\t" +majorversion + "." + minorversion)
			MessageBox msg title:"Siege Max needs to be updated"
			dsFatalError = true
		)
		else if (minorversion as integer) > SIEGEMAX_TOOLKIT_MINOR_VERSION then
		(
			local msg = "Your SiegeMax tools are out of date!\n\n"
			msg += "SiegeMax will still continue to function\n"
			msg += "but you should update as soon as possible\n\n"
			msg += ("Your version is:\t\t"+SIEGEMAX_TOOLKIT_VERSION_STRING + "\n")
			msg += ("The current version is:\t" +majorversion + "." + minorversion)
			MessageBox msg title:"Siege Max needs to be updated"
			dsFatalError = false
		)
		else
		(
			format "Running SiegeMax version %\n" SIEGEMAX_TOOLKIT_VERSION_STRING
		)
	)
)
catch
(
)

if not dsFatalError then 
(
	scriptdir = GetDir #scripts

	if ProductIs3dsMax() then scriptdir += "/SiegeMax"
	
	fileIn (scriptdir+"/plugins/asp_mod.ms")
	fileIn (scriptdir+"/plugins/sno_mod.ms")
	
	fileIn (scriptdir+"/NoteTrackTools.ms")
	fileIn (scriptdir+"/HelpTools.ms")
	fileIn (scriptdir+"/AttackTools.ms")
	fileIn (scriptdir+"/AnimKeyTools.ms")
	fileIn (scriptdir+"/BBoxTools.ms")
	fileIn (scriptdir+"/RootTools.ms")
	fileIn (scriptdir+"/SNOTools.ms")
	fileIn (scriptdir+"/ASPTools.ms")
	fileIn (scriptdir+"/StitchTools.ms")
	fileIn (scriptdir+"/RefObjTools.ms")
	
	-- Create the standard quad menus
	fileIn (ScriptDir+"/QuadMenus.ms")
	
	-- Run any one-shot scripts and then delete them
	RunOnceFiles = GetFiles (ScriptDir+"/RunOnce/*.ms")
	for i = 1 to RunOnceFiles.count do
	(
		fileIn RunOnceFiles[i]
		deleteFile RunOnceFiles[i]
	)
	
	-- Run any additional files that have been added to SiegeMax
	AdditionalFiles = GetFiles (ScriptDir+"/Add-Ons/*.ms")
	for i = 1 to AdditionalFiles.count do
	(
		fileIn AdditionalFiles[i]
	)
	
)




