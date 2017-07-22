Set objFSO		= CreateObject("Scripting.FileSystemObject")
Set sMap		= CreateObject("Scripting.Dictionary")
Set hMap		= CreateObject("Scripting.Dictionary")
Set fMap		= CreateObject("Scripting.Dictionary")
Set uMap		= CreateObject("Scripting.Dictionary")

' Scan will start from here.
root			= "D:\Dropbox\C++\"
' Project name.
name			= "Copper"

' Visual C\C++ project file.
Set proj		= objFSO.CreateTextFile(root & "\\" & name & ".vcxproj", True)
' Visual C\C++ filters(folders) file.
Set filters		= objFSO.CreateTextFile(root & "\\" & name & ".vcxproj.filters", True)

'
' *******************************************************************************************
' ********************************* PROJECT *************************************************
' *******************************************************************************************
'

'
' Retrieve the interesting information...
'

ScanInterestingFiles objFSO.GetFolder(root)

'
' Prepare the project file header.
'
proj.Write _
"<?xml version=""1.0"" encoding=""utf-8""?>" & vbCrLf & _
"<Project DefaultTargets=""Build"" ToolsVersion=""12.0"" xmlns=""http://schemas.microsoft.com/developer/msbuild/2003"">" & vbCrLf & _
"    <ItemGroup Label=""ProjectConfigurations"">" & vbCrLf & _
"        <ProjectConfiguration Include=""Debug|Win32"">" & vbCrLf & _
"            <Configuration>Debug</Configuration>" & vbCrLf & _
"            <Platform>Win32</Platform>" & vbCrLf & _
"        </ProjectConfiguration>" & vbCrLf & _
"    </ItemGroup>" & vbCrLf & _
"    <PropertyGroup Label=""Globals"">" & vbCrLf & _
"        <ProjectGuid>" & UUID & "</ProjectGuid>" & vbCrLf & _
"        <Keyword>MakeFileProj</Keyword>" & vbCrLf & _
"    </PropertyGroup>" & vbCrLf & _
"    <Import Project=""$(VCTargetsPath)\\Microsoft.Cpp.Default.props"" />" & vbCrLf & _
"    <PropertyGroup Condition=""'$(Configuration)|$(Platform)'=='Debug|Win32'"" Label=""Configuration"">" & vbCrLf & _
"        <ConfigurationType>Makefile</ConfigurationType>" & vbCrLf & _
"        <UseDebugLibraries>true</UseDebugLibraries>" & vbCrLf & _
"        <PlatformToolset>v120</PlatformToolset>" & vbCrLf & _
"    </PropertyGroup>" & vbCrLf & _
"    <Import Project=""$(VCTargetsPath)\\Microsoft.Cpp.props"" />" & vbCrLf & _
"    <ImportGroup Label=""ExtensionSettings"">" & vbCrLf & _
"    </ImportGroup>" & vbCrLf & _
"    <PropertyGroup Label=""UserMacros"" />" & vbCrLf & _
"    <ItemDefinitionGroup>" & vbCrLf & _
"    </ItemDefinitionGroup>" & vbCrLf & _
"    <Import Project=""$(VCTargetsPath)\\Microsoft.Cpp.targets"" />" & vbCrLf & _
"        <ImportGroup Label=""ExtensionTargets"">" & vbCrLf & _
"    </ImportGroup>" & vbCrLf & _
"    <PropertyGroup Condition=""'$(Configuration)|$(Platform)'=='Debug|Win32'"">" & vbCrLf & _
"        <NMakePreprocessorDefinitions></NMakePreprocessorDefinitions>" & vbCrLf & _
"        <NMakeIncludeSearchPath>" & vbCrLf

FormatProjectIncludes proj

proj.Write _
"        </NMakeIncludeSearchPath>" & vbCrLf & _
"        <ExecutablePath />" & vbCrLf & _
"        <IncludePath />" & vbCrLf & _
"        <ReferencePath />" & vbCrLf & _
"        <LibraryPath />" & vbCrLf & _
"        <LibraryWPath />" & vbCrLf & _
"        <SourcePath></SourcePath>" & vbCrLf & _
"        <ExcludePath />" & vbCrLf & _
"    </PropertyGroup>" & vbCrLf

' Insert the project elements.
FormatProjectElements proj

proj.Write _
"</Project>" & vbCrLf

'
' *******************************************************************************************
' ********************************* FILTERS *************************************************
' *******************************************************************************************
'

filters.Write _
"<?xml version=""1.0"" encoding=""utf-8""?>" & vbCrLf & _
"<Project ToolsVersion=""4.0"" xmlns=""http://schemas.microsoft.com/developer/msbuild/2003"">" & vbCrLf & _
"    <ItemGroup>"& vbCrLf & _
"        <Filter Include=""Source Files"">" & vbCrLf & _
"            <UniqueIdentifier>{" & UUID() & "}</UniqueIdentifier>" & vbCrLf & _
"        </Filter>" & vbCrLf & _
"        <Filter Include=""Header Files"">" & vbCrLf & _
"            <UniqueIdentifier>{" & UUID() & "}</UniqueIdentifier>" & vbCrLf & _
"        </Filter>" & vbCrLf & _
"        <Filter Include=""Resource Files"">" & vbCrLf & _
"            <UniqueIdentifier>{" & UUID() & "}</UniqueIdentifier>" & vbCrLf & _
"        </Filter>" & vbCrLf

' Copy the structure for Headers.
For Each k in fMap

	filters.WriteLine _
"        <Filter Include=""Header Files" & k & """>" & vbCrLf & _
"            <UniqueIdentifier>{" & UUID() & "}</UniqueIdentifier>" & vbCrLf & _
"        </Filter>"

Next

' Copy the structure for Sources.
For Each k in fMap

	filters.WriteLine _
"        <Filter Include=""Source Files" & k & """>" & vbCrLf & _
"            <UniqueIdentifier>{" & UUID() & "}</UniqueIdentifier>" & vbCrLf & _
"        </Filter>"

Next

filters.Write _
"    </ItemGroup>"& vbCrLf

' Format all the elements on their filters.
FormatFilterElements filters

filters.Write vbCrLf & _
"</Project>"

'
' *******************************************************************************************
' ********************************* PROCEDURES **********************************************
' *******************************************************************************************
'

' Writes the project elements in the desired format.
Sub FormatFilterElements(Output)

	Output.Write _
"    <ItemGroup>" & vbCrLf

	' Inserts the headers.
	For Each k in hMap

		If Not IsEmpty(CStr(k)) Then

			Output.WriteLine _
"        <ClInclude Include=""." & k & """>" & vbCrLf & _
"            <Filter>Header Files" & hMap.Item(k)  & "</Filter>" & vbCrLf & _
"        </ClInclude>"

		End IF
	Next

	Output.Write _
"    </ItemGroup>" & vbCrLf & _
"    <ItemGroup>" & vbCrLf

	' Inserts the sources.
	For Each k in sMap
		Output.WriteLine _
"        <ClCompile Include=""." & k  & """>" & vbCrLf & _
"            <Filter>Source Files" & sMap.Item(k)  & "</Filter>" & vbCrLf & _
"        </ClCompile>"
	Next

	Output.Write _
"    </ItemGroup>" & vbCrLf

End Sub

' Writes the project elements in the desired format.
Sub FormatProjectElements(Output)

	Output.Write _
"    <ItemGroup>" & vbCrLf

	' Inserts the headers.
	For Each k in hMap
		Output.WriteLine _
"        <ClInclude Include=""." & k & """ />"	
	Next

	Output.Write _
"    </ItemGroup>" & vbCrLf & _
"    <ItemGroup>" & vbCrLf

	' Inserts the sources.
	For Each k in sMap
		Output.WriteLine _
"        <ClCompile Include=""." & k & """ />"	
	Next

	Output.Write _
"    </ItemGroup>" & vbCrLf

End Sub

' Writes the includes in the desired format.
Sub FormatProjectIncludes(Output)

	' Get this folder sub-folders.
    For Each k in fMap

        Output.WriteLine _
"            ." & k & ";"

    Next

End Sub

Sub ScanInterestingFiles(Folder)

	WScript.Echo "Scanning folder " & Folder.Path & "..."

	' Get this folder files... 
	Set colFiles = Folder.Files

	' ... and then process them.
    For Each objFile in colFiles

		' Proceed if the file has an extension.
		If (InStr(objFile.Name, ".") > 0) Then
			
			' Evaluate the extension.
			ext = LCase(Mid(objFile.Name, InStrRev(objFile.Name, ".")))
			 
			' Headers.
			If ext = ".h" Or _
			ext = ".hh" Or _
			ext = ".hpp" Or _
			ext = ".inl" Or _
			ext = ".inc" Or _
			ext = ".hxx" _
			Then
				hMap.Add Replace(objFile.Path, root, ""), Replace(Folder.Path, root, "")
			End If

			' Sources.
			If ext = ".c" Or _
			ext = ".cpp" Or _
			ext = ".cc" Or _
			ext = ".cxx" Or _
			ext = ".s" Or _
			ext = ".asm" _
			Then
				sMap.Add Replace(objFile.Path, root, ""), Replace(Folder.Path, root, "")
			End If

		End If

    Next

	f = Replace(Folder.Path, root, "")

	If fMap.Exists(f) = False Then
		
		' This in an interesting folder. Save it.
		fMap.add f, ""

	End If

	' Get this folder sub-folders.
    For Each Subfolder in Folder.SubFolders

        ' Continue with such procedure recursively.
		ScanInterestingFiles Subfolder

    Next

End Sub

'
' This will help us generate UUIDs.
'

Randomize Timer

' Returns a new UUID 
'
Function UUID()

	Dim i, RndNum

	For i = 0 to 7

		RndNum = CLng(rnd * "&HFFFF")

		'if i = 0 then RndNum = RndNum Xor (CLng(Date) And "&HFFFF")
		If i = 3 Then 
			RndNum = (RndNum And "&HFFF") Or "&H4000"
		End If

		If i = 4 Then 
			RndNum = (RndNum And "&H3FFF") Or "&H8000"
		End If

		UUID = UUID + String(4 - Len(Hex(RndNum)), "0") + LCase(Hex(RndNum))

		If i=1 Or i=2 Or i=3 Or i=4 Then 
			UUID = UUID & "-"
		End If

	Next

	' Recompute in case of already existing UUID.
	If uMap.Exists(UUID) Then
		UUID = UUID()
	Else
		uMap.Add UUID, ""
	End If

End Function

