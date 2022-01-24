'use strict';

module.exports = function(grunt) {

  // Please see the Grunt documentation for more information regarding task
  // creation: http://gruntjs.com/creating-tasks
  
  function methodIsConstructor(str, className) {
	  if (str.indexOf(className) != 0) {
		  return false;
	  }
	  for (var i = className.length; i < str.length; i++) {
		  var chr = str.charAt(i);
		  if (chr !== ' ' && chr !== '(') {
			  return false;
		  }
		  if (chr === '(') {
			  return true;
		  }
	  }
	  return false;
  }
  
  function getMethodInfo(method) {
	  var methodDescArr = method.split(" ");
	  var info = [];
	  
	  function doResult() {
		  if (!info.length) return false;
		  if (info.length == 1) {
			  return info;
		  }
		  info = [
				info.slice(0, info.length-1)
					.map(function(s){
						return s.trim();
					})
					.join(' '),
				info[info.length-1]
			];
		  
		  info.push(info[0].length);
		  if (info[0].indexOf('virtual') > -1) {
			//grunt.log.writeln(JSON.stringify(info));
			info[0] = info[0].replace('virtual ', '').trim();
			//grunt.log.writeln(JSON.stringify(info));
		  }
		  if (info[0].indexOf('static') > -1) {
			info[0] = info[0].replace('static ', '').trim();
		  }
		  return info;
	  }

	  for (var i = 0; i < methodDescArr.length; i++) {
		  var test = methodDescArr[i];
		  if (test === '(') {
			  info = methodDescArr.slice(0, i);
			  return doResult();
		  }
		  else {
			  var br = test.indexOf('(');
			  if (br > -1) {
				  info = methodDescArr.slice(0, i);
				  info.push(test.substring(0, br).trim());
				  return doResult();
			  }
		  }
	  }
	  return false;
  }

  grunt.registerMultiTask('grunt-h2cpp-parser', 'parse .h files and auto-generate .cpp files then needed', function() {
    // Merge task-specific and/or target-specific options with these defaults.
    var options = this.options({
      skipRegex: null,
      filePrefix: 'pre_',
      filePostfix: '_post'
    });
	
	var path = require('path');
	
	function fileToArr(filepath) {
		var fileStr = grunt.util.normalizelf(grunt.file.read(filepath));
        return fileStr.split(grunt.util.linefeed);
	}

    // Iterate over all specified file groups.
    this.files.forEach(function(f) {
      // Concat specified files.
      //var src = options.filePrefix ? (options.filePrefix + grunt.util.linefeed) : '';
      f.src.filter(function(filepath) {
		var filename = path.basename(filepath);
		var fileext = path.extname(filepath);
		if (fileext != '.h' && fileext != '.hcc') {
			return false;
		}
		
        // Warn on and remove invalid source files (if nonull was set).
        if (!grunt.file.exists(filepath)) {
          grunt.log.warn('Source file "' + filepath + '" not found.');
          return false;
        } else {
          return true;
        }
      }).map(function(filepath) {
		grunt.log.writeln('processing file ' + filepath );
		
		var fileext = path.extname(filepath);
		var filename = path.basename(filepath, fileext);
		var dir = path.dirname(filepath);
		
		// prepare .cpp
		var cppFilename = filename + '.cpp';
		var cppFilepathOrig = dir + '/' + cppFilename;
		var cppFileData = [];	// src data, created from .cpp
		var cppFileDataNew = [];	// to append from .h file
		var hFileDataNew = [];	// converted .h file data
		var includes = {};	// "f.h" to: 1 - old include, 2 - included from @include in .h
		var className = false;
		
		grunt.log.writeln('name: ' + filename + ', ext: ' + fileext + ', cpp: ' + cppFilepathOrig);
	
		
		if (grunt.file.isFile(cppFilepathOrig)) {
			// parse old cpp
			cppFileData = fileToArr(cppFilepathOrig);
			cppFileData.map(function(line) {
				//grunt.log.writeln(line);
				var chunks = line.trim().split(" ");
				if (chunks.length == 2 && chunks[0] == '#include') {
					includes[chunks[1]] = 1;
				}
			});
		} else {
			includes['"' + filename + '.h"'] = 2;
		}
		
		// go trough h-file
		var srcData = fileToArr(filepath);
		var implementations = [];
		
		//var funcName = false;
		var funcBody = false;
		var openBrackets = 0;
		var methodDesc = false;
		var linePreCutCpp = false;
		var move2cpp = 0;
		
		function closeBracket() {
			openBrackets--;
			if (openBrackets == 0) {
				// funcBody end!
				//implementations.push([methodDesc, funcBody]);
				funcBody = false;
				methodDesc = false;
				cppFileDataNew.push('');
			}
		}
		
		srcData.map(function(line) {
			var str = line.trim();
			var chunks = str.split(" ");
			if (chunks[0] === 'class') {
				className = chunks[1];
				if (className.indexOf(':') > -1) {
					className = className.substr(0, className.indexOf(':'));
				}
			}
			var isComment = false;
			if (str.charAt(0) + str.charAt(1) === '//') {
				isComment = true;
			}
			var comment = str.substring(2).trim();
			if (isComment) {
				//grunt.log.writeln('comment: ' + comment);
			}
			if (isComment && comment.charAt(0) === '@') {
				// update chunks
				chunks = comment.trim().split(" ");
				//grunt.log.writeln('t: ' + chunks[0]);
				
				if (chunks[0] === '@implement') {
					// start implementation
					funcBody = true;
					openBrackets = 0;
					methodDesc = false;
					linePreCutCpp = false;
				} else if (chunks[0] === '@include') {
					includes[chunks[1]] = 2;
				} else if (chunks[0] === '@cpp') {
					move2cpp = 1;
				}
				return;
			}
			if (move2cpp > 0) {
				cppFileDataNew.push(line);
				if (move2cpp == 1) {
					move2cpp = 0;
				}
				return;
			}
			if (!isComment && funcBody) {
				if (!methodDesc) {
					// not yet started
					var bracket = str.indexOf('{');
					
					if (bracket > -1) {
						methodDesc = str.substr(0, bracket).trim();
						str = str.substr(bracket);
					} else {
						// no open bracket
						methodDesc = str;
						str = '';
					}

					var dots = methodDesc.indexOf(':');
					if (dots > -1 && methodDesc[dots+1] != ':') {
						str = methodDesc.substr(dots).trim() + str;
						methodDesc = methodDesc.substr(0, dots).trim();
					}
					
					var methodInfo = getMethodInfo(methodDesc);
				
					linePreCutCpp = line.substr(0, line.indexOf(methodDesc));
					if (methodInfo.length == 1) {	//methodIsConstructor(methodDesc, className) // class constructor
						line = (className ? (className + "::") : '') + methodDesc + " " + str; //str.substr(bracket);
					} else {
						//grunt.log.writeln(methodDesc.substr(methodInfo[2]).trim());
						line = methodInfo[0] + " " + (className ? (className + "::") : '') + methodDesc.substr(methodInfo[2]).trim() + " " + str;	//methodInfo[0].length
					}
					
					hFileDataNew.push(linePreCutCpp + methodDesc + ';');
				}
				if (!line) {
					return;
				}
				if (line.indexOf(linePreCutCpp) === 0) {
					line = line.substr(linePreCutCpp.length);
				}
				cppFileDataNew.push(line);
				
				// calc brackets
				for (var i = 0; i < line.length; i++) {
					var chr = line.charAt(i);
					if (chr === '{') {
						openBrackets++;
						//grunt.log.write('{');
					} else if (chr === '}') {
						/*grunt.log.write('}');
						if (openBrackets < 2) {
							grunt.log.writeln(methodDesc + ': openBrackets: '+openBrackets+', funcBody: ' + funcBody);
						}*/
						closeBracket();
					}
				}
			} else if (isComment && funcBody) {
				if (line.indexOf(linePreCutCpp) === 0) {
					line = line.substr(linePreCutCpp.length);
				}
				cppFileDataNew.push(line);
			} else {
					hFileDataNew.push(line);
				}
		});
		
		//grunt.log.writeln('className: ' + className);
		
		////////////////

		grunt.log.writeln('Dest: ' + f.dest);
		
		grunt.log.writeln('cppFileData.length: ' + cppFileData.length);
		grunt.log.writeln('cppFileDataNew.length: ' + cppFileDataNew.length);
		
		if (cppFileData.length + cppFileDataNew.length) {
			//h
			grunt.file.write(f.dest + "/" + filename + '.h', hFileDataNew.join(grunt.util.linefeed));
			grunt.log.writeln('File "' + f.dest + "/" + filename + '.h' + '" created.');
			
			//cpp
			Object.keys(includes).forEach(function(include) {
				if (includes[include] === 2) {
					cppFileData.push('#include ' + include);
				}
			});
			
			cppFileData.push('');
			
			grunt.file.write(f.dest + "/" + cppFilename, cppFileData.concat(cppFileDataNew).join(grunt.util.linefeed));
			grunt.log.writeln('File "' + f.dest + "/" + cppFilename + '" created.');
		}
		
		funcBody = false;
		methodDesc = false;
		
		if (fileext == '.hcc') {
			// remove .hcc
			grunt.file.delete(filepath);
			grunt.log.writeln('PRE-source .HCC file COPY "' + filepath + '" DELETED.');
		}

        return '---' + filepath;
      });

      //src += options.filePostfix ? (grunt.util.linefeed + options.filePostfix) : '';

      //grunt.log.writeln('dest file:' + f.dest);
      //grunt.log.debug(src);
      // Write the destination file.
      //grunt.file.write(f.dest, src);

      // Print a success message.
      //grunt.log.writeln('File "' + f.dest + cppFilename + '" created.');
    });
  });

};
