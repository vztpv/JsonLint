[![Build status](https://ci.appveyor.com/api/projects/status/56ori3o7534vs89f?svg=true)](https://ci.appveyor.com/project/vztpv/jsonlint)

# JsonLint - No Support For UTF-8, Unicodes..., 
# // only for ANSI !, check test folder.
    Using ClauText API, like ClauLint (only semantic? validator?)
    No Grammar Check! ??
    Supoort Parallel Parsing.
    Can Check More specific Semantic validation??
    (using clautext code?, ex) integer -> even? or odd? )
# jsonLint.exe -V test_file_name

# ClauText Code
    $Event = { # line comment.
      id = semantic_chk_event_name
      
      $parameter = { name is_usertype real_dir }
      
      $return = { TRUE }
    }
# JsonLint Semantic Check Example??
    {
        "xxx" = %int  # integer type
        "yyy" = 3  # yyy = 3

        "x" = { 
          "a"%can_empty_ut = { 
             %order_on %multiple_on
             %can_empty_ut%event_semantic_chk_event_name = { %order_on  %multiple_on %int } 
          }  
        }
    }
    
 # Test Json File ??
    {
      "xxx" : 4,
      "yyy" : 3,

      "x" : {
        "a" :  {
          "111" : [ 33, 44, 55, 66 ], 
          "222" : [ 11, 22, 33 ]
        }
      }
    }
