'use strict';

module.exports = function(grunt) {

	// Project configuration.
	grunt.initConfig({
		// Before generating any new files, remove any previously-created files.
		clean: {
			build: ['../HTTPClient32']
		},

		copy: {
			main: {
				files: [
					// includes files within path and its sub-directories
					{
					  expand: true, cwd: 'src', src: '**', dest: '../HTTPClient32'
					}
				]
			}
		},

		run: {
		},

		// Configuration to be run (and then tested).
		"grunt-h2cpp-parser": {
		  parsesrc: {
			files: {
			  '../HTTPClient32': ['../HTTPClient32/*.hcc']
			}
		  }
		}
	});

	// Actually load this plugin's task(s).
	grunt.loadTasks('tasks');

	// These plugins provide necessary tasks.
	grunt.loadNpmTasks('grunt-contrib-clean');
	grunt.loadNpmTasks('grunt-contrib-copy');
	//grunt.loadNpmTasks('grunt-run');

	grunt.registerTask('build', ['clean','copy','grunt-h2cpp-parser']);

	// By default, lint and run all tests.
	grunt.registerTask('default', ['build']);
};
