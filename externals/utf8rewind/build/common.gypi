{
	'variables': {
		'conditions': [
			['OS=="win"', {
				'platform_name%': 'windows',
				'architecture_name%': '$(PlatformName)',
			}],
			['OS=="mac"', {
				'platform_name%': 'macosx',
				'architecture_name%': 'x86', # TODO: Impossible to determine architecture per configuration?
			}],
			['OS=="linux"', {
				'platform_name%': 'linux',
				'architecture_name%': 'x86', # TODO: Impossible to determine architecture per configuration?
			}],
		],
	},
	'target_defaults': {
		'default_configuration': 'Debug',
		'configurations': {
			'Common': {
				'abstract': 1,
				'conditions': [
					['OS=="win"', {
						'defines': [ '_WINDOWS', '_CRT_SECURE_NO_WARNINGS' ],
						'msvs_configuration_attributes': {
							'OutputDirectory': '$(SolutionDir)output\\<(platform_name)\\<(architecture_name)\\$(ConfigurationName)',
							'IntermediateDirectory': '$(SolutionDir)intermediate\\$(ProjectName)\\<(architecture_name)\\$(ConfigurationName)',
							'CharacterSet': '1', # unicode
						},
						'msvs_settings': {
							'VCCLCompilerTool': {
								'WarningLevel': 3, # /W3
								# 'WarnAsError': 'true' # TODO
							},
							'VCLibrarianTool': {
								'OutputFile': '$(OutDir)$(TargetName)$(TargetExt)',
							},
							'VCLinkerTool': {
								'OutputFile': '$(OutDir)$(TargetName)$(TargetExt)',
							},
						},
					}],
					['OS=="linux"', {
						'cflags': [
							'-g',
							'-Wall',
							'-Wextra',
							'-Wno-missing-field-initializers', # don't warn on initializing structs with "= { 0 };"
						],
						'cflags_cc': [
							'-std=c++11'
						],
					}],
				],
			},
			'Platform_x86': {
				'abstract': 1,
				'conditions': [
					['OS=="win"', {
						'defines': [ 'WIN32' ],
						'msvs_configuration_platform': 'Win32',
						'msvs_settings': {
							'VCLinkerTool': {
								'TargetMachine': '1', # MachineX86
							},
						},
					}],
					# TODO: Architecture configuration on other platforms.
				]
			},
			'Platform_x64': {
				'abstract': 1,
				'conditions': [
					['OS=="win"', {
						'defines': [ '_WIN64' ],
						'msvs_configuration_platform': 'x64',
						'msvs_settings': {
							'VCLinkerTool': {
								'TargetMachine': '17', # MachineX64
							}
						},
					}],
					['OS=="linux"', {
						'cflags': [ '-m64' ],
					}],
					# TODO: Architecture configuration on other platforms.
				],
			},
			'Debug_Base': {
				'abstract': 1,
				'defines': [ 'DEBUG', '_DEBUG' ],
				'conditions': [
					['OS=="win"', {
						'msvs_settings': {
							'VCCLCompilerTool': {
								'Optimization': 0, # /Od
								'BasicRuntimeChecks': 3, # /RTC1
								'RuntimeLibrary': 3, # /MDd (dynamic debug)
								'ProgramDataBaseFileName': '$(TargetDir)$(ProjectName).pdb',
							},
							'VCLinkerTool': {
								'LinkIncremental': 2, # /INCREMENTAL
								'GenerateDebugInformation': 'true',
							},
						},
					}],
				],
			},
			'Release_Base': {
				'abstract': 1,
				'defines': [ 'NDEBUG' ],
				'conditions': [
					['OS=="win"', {
						'msvs_settings': {
							'VCCLCompilerTool': {
								'RuntimeLibrary': 2, # /MD (dynamic release)
								'Optimization': 3, # /Ox
								'InlineFunctionExpansion': 2, # /Ob2
								'EnableIntrinsicFunctions': 'true', # /Oi
								'FavorSizeOrSpeed': 1, # /Ot
								'EnableEnhancedInstructionSet': 2, # /arch:SSE2
								'FloatingPointModel': 2, # /fp:fast
							},
							'VCLinkerTool': {
								'LinkIncremental': 2, # /INCREMENTAL
							},
						},
					}],
				],
			},
			'Debug': {
				'inherit_from': ['Common', 'Platform_x86', 'Debug_Base' ],
			},
			'Release': {
				'inherit_from': ['Common', 'Platform_x86', 'Release_Base' ],
			},
			'Debug_x64': {
				'inherit_from': ['Common', 'Platform_x64', 'Debug_Base' ],
			},
			'Release_x64': {
				'inherit_from': ['Common', 'Platform_x64', 'Release_Base' ],
			},
		},
	},
}