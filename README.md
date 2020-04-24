[![Build status](https://ci.appveyor.com/api/projects/status/56ori3o7534vs89f?svg=true)](https://ci.appveyor.com/project/vztpv/jsonlint)

# JsonLint - Support For UTF-8, ANSI 

# JsonLint.exe -V test_file_name

# Test File

    data = {
      #
    }
    schema = {
      #
    }

    # $encoding = UTF-8
    Event = {
      id = __init__  # special event. // before validation.

      # for output to console, output to file.
      $encoding_utf-8 = { } 
          # in jsonLint Defualt is utf-8?

      # LoadData
      $load = { "C:\Users\vztpv\Desktop\Clau\JsonLint\JsonLint\b.txt" } 

      # LoadData->LoadData2
      $load_json = { { "C:\Users\vztpv\Desktop\Clau\JsonLint\JsonLint\a.json" }  { data } }
      #$load_json = { { "C:\Users\vztpv\source\repos\ClauJson\ClauJson\input.json" }  { data } }
      #  - LoadData2? : 333'dddd'333 -> one token, 
      #  - # line comment, // line comment, /* */ comment,
      # clau test parer + clauText + claujson..

      $load_my_json_schema = { { "C:\Users\vztpv\Desktop\Clau\JsonLint\JsonLint\schema.txt" } { schema } }

      $print = { { "init.." } }
      $print = { enter = { \n } }

      # $print2 = { { /./ } }

      $return = { data schema } # returns folder names?
    }

    Event = {
      id = __end__ # special event. // after validation

      $print = { enter = { \n } }
      $print = { data = { "end__" } }
      $print = { enter = { \n } }
      $_getch = { }
    }

    x = 10

    Style = {
      id = x

      %multiple
      %optional
      %event_int_string = { }
      %event_list_sum_larger_than = { n = /./x }
    }

    Enum = {
      id = x

      1 2 3 4
    }

    Event = { 
      id = list_sum_larger_than_10

      $local = { sum i }
      $parameter = { name value is_usertype real_dir } 

      $assign = { $local.sum { 0 } }
      $assign = { $local.i { 0 } }

      $while = { 
        cond = {                
        # num of itemtype ( var = val or val )
          $COMP< = { $local.i $size = { $parameter.real_dir } }
        }
        then = {
          $assign = { $local.sum { $add = { $local.sum 
          $get = { $concat_all = {
            $parameter.real_dir / $ it  $local.i }
          } } } } 
          $assign = { $local.i { $add = { $local.i 1 } }
          }
        }
      }
      $if = { 
        cond = {
          $COMP> = {
            $local.sum
            10
          }
        }
        then = {
          $return = { TRUE }
        }
      }
      $return = { FALSE }
    }

    Event = { 
      id = larger_than
      $parameter = { n name value is_usertype real_dir select } # select : NAME or VALUE ? 

      $if = { 
        cond = {
          $COMP> = {
            $parameter.value 
            $parameter.n
          }
        }
        then = {
          $return = { TRUE }
        }
      }

      $return = { FALSE }
    }
    Event = { 
      id = larger_than_300
      $parameter = { name value is_usertype real_dir select } # select : NAME or VALUE ? 
      $if = { 
        cond = {
          $COMP> = {
            $parameter.value 
            300
          }
        }
        then = {
          $return = { TRUE }
        }
      }

      $return = { FALSE }
    }
    Event = {
      id = int_string
      $parameter = { name value input
              is_usertype real_dir } 
      $if = { 
        cond = {
          $EQ = { 
            INTEGER $get_type = { 		  
              $remove_quoted = { 
              $parameter.input }
            }
          }
        }
        then = {
          $return = { TRUE }
        }
      }
      $return = { FALSE }
    }

    Event = {
      id = int
      $parameter = { name value input
              is_usertype real_dir } 

      $if = { 
        cond = {
          $EQ = { 
            INTEGER $get_type = { 		  
              #$remove_quoted = { 
              $parameter.input# }
            }
          }
        }
        then = {
          $return = { TRUE }
        }
      }
      $return = { FALSE }
    }


# JsonLint Semantic Check Code Example
    {
      $log : on, // comment 
      #"abc" : "aaaa"dddd'eee'"333",
      "xxx"%optional : %int%event_larger_than@'n = 300',  # ' '
      "yyy" : 3,

      "x" : {
        "z" : { 
          %event_int_string%multiple : [ 3 ],

          "c" : [ %int%id_B ] ,
          "d" : [ %int%id_B ] , 
          /*  
            comment
          */
          "e" : [ %int%id_B , %int%id_B ]		
        },
        %multiple : {
          %style_x : [		
            %multiple%event_int%enum_x%id
            #%id_A, %id_B
          ]
        }
      }
    }

# Test Json File ( "a.json" )

    {
      "xxx": 333,
      "yyy": 3,
      "x": {
        "z": {
          "1": [3],
          "2": [3],
          "c": [4] ,
          "d": [5],
          "e": [6, 8]

        },
        "a,a": {
          "3": [20],
          "4": [1, 2, 3, 5, 15],
          "566": [44 ,154]
        },
        "a,b": {
          "66": [-122 ,334] 
        }
      }
    }
