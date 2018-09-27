# JsonLint - Not Yet.
    Using ClauText API, like ClauLint (only semantic? validator?)
    No Grammar Check! ??
    Supoort Parallel Parsing.
    Can Check More specific Semantic validation??
    (using clautext code?, ex) integer -> even? or odd? )
    
# ClauText Code
    $Event = { # line comment.
      id = semantic_chk_event_name
      
      $parameter = { name is_usertype real_dir }
      
      $return = { TRUE }
    }
# JsonLint Semantic Check Example??
    
    "xxx" = %int  # integer type
    "yyy" = 3  # yyy = 3

    "x" = { 
      "a"%can_empty_ut = { 
         %order_on %multiple_on
         %can_empty_ut%int%semantic_chk_event_name = { %order_on  %multiple_on %int } 
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
