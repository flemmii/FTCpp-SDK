// Imports
const fs = require("node:fs");
const path = require("node:path");

// Paths
const Mode = {
  Autonomous: "Autonomous.java.template",
  TeleOp: "TeleOp.java.template",
};

// Escapers
const groupEscaper = "<group>";
const preselectTeleOpEscaper = "<preselectTeleOp>";
const mainMethodNameEscaper = "<mainMethodName>";

// User input check
const pascalCaseRegex = /^[A-Z][a-zA-Z0-9]*$/;
const basicNotTroubleCausingCharacters = /^[a-zA-Z0-9 _\-$/]*$/;

const displayNameEscaper = "<displayName>";
const displayNameRegex = basicNotTroubleCausingCharacters;
function testDisplayName(name) {
  if (!displayNameRegex.test(name)) {
    throw new Error(`Display Name: "${name}" Not Valid`);
  }
}

const classNameEscaper = "<className>";
const classNameRegex = pascalCaseRegex;
function testClassName(name) {
  if (!classNameRegex.test(name)) {
    throw new Error(`Class Name: "${name}" Not Valid`);
  }
}

const packageEscaper = "<package>";
const headerFilesToScanPath = path.join(__dirname, "..", "teamcode/header");

// Recursive File Scan
function getAllFiles(dir, fileArray) {
  const files = fs.readdirSync(dir);

  fileArray = fileArray || [];

  files.forEach((file) => {
    const filePath = path.join(dir, file);
    if (fs.statSync(filePath).isDirectory()) {
      fileArray = getAllFiles(filePath, fileArray);
    } else {
      fileArray.push(filePath);
    }
  });

  return fileArray;
}

// Cpp file extraction
const javaPathPrefix = path.join(__dirname, "..", "..", "java");
const javaPathRegex = /(org_firstinspires_[a-zA-Z0-9_]+)/;
const javaFileSuffix = ".java";

const ignoreFlagRegex = /@Disabled/;
const informationLineRegex =
  /@(?<mode>TeleOp|Autonomous)(?:(?:[ \n])|(?:\((?<content>.*)\)))/g;
const informationLineDisplayNameContentRegex =
  /(?:(?: +|)name(?: +|)=(?: +|))\\?"(?<displayNameContent>[^\\?"]+)\\?"(?: +|)/g;
const groupRegex =
  /(?:(?: +|)group(?: +|)=(?: +|))\\?"(?<group>[^\\?"]+)\\?"(?: +|)/g;
const preselectTeleOpRegex =
  /(?:(?: +|)preselectTeleOp(?: +|)=(?: +|))\\?"(?<preselectTeleOp>[^\\?"]+)\\?"(?: +|)/g;
const separatingCommasAndSpacesRegex = /[ ,]+/g;

function mapCppFilesToBeingJavaFileInformation(filePathArray) {
  let javaPathArray = [];
  filePathArray.forEach((filePath) => {
    let singleFileContent = fs.readFileSync(filePath, "utf-8");

    if (ignoreFlagRegex.test(singleFileContent)) {
      return;
    }

    let matchResult = singleFileContent.match(javaPathRegex);
    if (matchResult === null) {
      return;
    }
    console.log(matchResult);
    let javaPathSuffix = matchResult[0].split("_");
    let mainMethodName = javaPathSuffix.pop();
    let javaClassName = javaPathSuffix.pop();
    let package = javaPathSuffix.join(".");
    javaPathSuffix = javaPathSuffix.join("/");
    let javaFilePath = path.join(javaPathPrefix, javaPathSuffix);
    let javaFileName = javaClassName + javaFileSuffix;

    let informationLine = new RegExp(informationLineRegex).exec(
      singleFileContent
    );
    if (informationLine === null) {
      throw new Error(
        //if something gets recognized as a opMode but does not contain the info line
        `Error Found in File: ${filePath}
          Information Line ex.  @Autonomous(name = "Cpp Test")  not found or containing syntax issues`
      );
    }
    let mode = informationLine.groups["mode"];
    let informationLineContent = informationLine.groups["content"];
    let displayName = new RegExp(informationLineDisplayNameContentRegex).exec(
      informationLineContent
    );
    if (displayName === null) {
      displayName = javaClassName;
    } else {
      displayName = displayName.groups["displayNameContent"];
    }
    let group = new RegExp(groupRegex).exec(informationLineContent);
    if (group !== null) {
      group = group.groups["group"];
    }
    let preselectTeleOp = new RegExp(preselectTeleOpRegex).exec(
      informationLineContent
    );
    if (preselectTeleOp !== null) {
      preselectTeleOp = preselectTeleOp.groups["preselectTeleOp"];
    }
    let stringToCheckSyntax = informationLineContent
      .replaceAll(informationLineDisplayNameContentRegex, "")
      .replaceAll(groupRegex, "")
      .replaceAll(preselectTeleOpRegex, "")
      .replaceAll(separatingCommasAndSpacesRegex, "");
    if (stringToCheckSyntax !== "") {
      throw new Error(
        `Error Found in File: ${filePath}
          Syntax Error in information line. Example of a correct one : @Autonomous(name = "AutonomousForTheWin", group = "Auto", preselectTeleOp = "DriverControlled")`
      );
    }
    javaPathArray.push({
      group,
      preselectTeleOp,
      mode,
      displayName,
      javaFilePath,
      javaClassName,
      javaFileName,
      package,
      mainMethodName,
    });
  });
  return javaPathArray;
}

// File Content Creation
function insertIntoTemplates(
  displayNameContent,
  classNameContent,
  mode,
  package,
  group,
  preselectTeleOp,
  mainMethodName
) {
  let filePath = path.join(__dirname, mode);
  let template = fs.readFileSync(filePath, "utf-8");
  testClassName(classNameContent);
  testDisplayName(displayNameContent);
  if (group === null) {
    group = "";
  } else {
    group = `, group = "${group}"`;
  }
  if (preselectTeleOp === null || mode == Mode.TeleOp) {
    preselectTeleOp = "";
  } else {
    preselectTeleOp = `, preselectTeleOp = "${preselectTeleOp}"`;
  }
  return template
    .replaceAll(packageEscaper, package)
    .replaceAll(displayNameEscaper, displayNameContent)
    .replaceAll(classNameEscaper, classNameContent)
    .replaceAll(preselectTeleOpEscaper, preselectTeleOp)
    .replaceAll(groupEscaper, group)
    .replaceAll(mainMethodNameEscaper, mainMethodName);
}

// General File Creator
function createFile(fileContent, fileName, filePath) {
  try {
    fs.mkdirSync(filePath, { recursive: true });
  } catch (error) {
    //if folder allergy exists
  }
  fs.writeFileSync(path.join(filePath, fileName), fileContent);
}

//Previous java file deletion
const javaFileIdentifier = "Do_not_remove_this_string";
function reductionToGeneratedOpModes(javaFilePathArray) {
  return javaFilePathArray.filter((filePath) => {
    let fileContent = fs.readFileSync(filePath, "utf-8");
    return fileContent.includes(javaFileIdentifier);
  });
}
function deleteFiles(filePathArray) {
  filePathArray.forEach((filePath) => {
    fs.unlinkSync(filePath);
  });
}
/*
Main
*/
// Java File Deletion
deleteFiles(reductionToGeneratedOpModes(getAllFiles(javaPathPrefix)));

// Java File Creation
let fileInformationArray = mapCppFilesToBeingJavaFileInformation(
  getAllFiles(headerFilesToScanPath)
);
fileInformationArray.forEach((fileInformation) => {
  let {
    group,
    preselectTeleOp,
    mode,
    displayName,
    javaFilePath,
    javaClassName,
    javaFileName,
    package,
    mainMethodName,
  } = fileInformation;
  let javaFileContent = insertIntoTemplates(
    displayName,
    javaClassName,
    Mode[mode],
    package,
    group,
    preselectTeleOp,
    mainMethodName
  );
  createFile(javaFileContent, javaFileName, javaFilePath);
});
