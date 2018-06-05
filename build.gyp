{

 'targets':[

         {
            'target_name':'ws_server',
            'type':'executable',
            'dependencies':[],
            'defines':[],
            'include_dirs':[
              './'
            ],
            'sources':[
                  'main.cpp',
                  'base64.h',
                  'base64.cpp',
                  'ConfigFileReader.cpp',
                  'ConfigFileReader.h',
                  'debug_log.cpp',
                  'debug_log.h',
                  'network_interface.h',
                  'network_interface.cpp',
                  'sha1.h',
                  'sha1.cpp',
                  'websocket_handler.cpp',
                  'websocket_handler.h',
                  'websocket_request.h',
                  'websocket_request.cpp',
                  'websocket_respond.h',
                  'websocket_respond.cpp'
                  
            ],
            
            'conditions':[

             ['OS=="linux"',
                {
                'cflags':[
                    '-std=c++11',
                 ],
                'ldflags':[]
                },{
                  'cflags':[],
                  'ldflags':[],
                  'libraries':[
                  ]
                }
                
             ]


            ]
           	
         } 
          
  
 

 ],

}

