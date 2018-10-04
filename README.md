[![Build status](https://ci.appveyor.com/api/projects/status/56ori3o7534vs89f?svg=true)](https://ci.appveyor.com/project/vztpv/jsonlint)

# JsonLint - No Support For UTF-8, Unicodes..., 
# // only for ANSI !
    Using ClauText API, like ClauLint (only semantic? validator?)
    No Grammar Check! ??
    Supoort Parallel Parsing.
    Can Check More specific Semantic validation??
    (using clautext code?, ex) integer -> even? or odd? )
# JsonLint.exe -V test_file_name

# JsonLint Semantic Check code Example??
    $test2 = {

    }

    $Event = {
        id = __init__  # special event.

        $load_json2 = { { "a.json" } # here, change!
        { test2 }  }
        $print = { data = { "init end.." } }
        $print = { enter = { \n } }
        $return = { test2 } # returns data folder?
    }


    $Event = { 
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
                $assign = { $local.i { $add = { $local.i 1 } } }
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

    $Event = { 
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
    $Event = {
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

    %log_on #%log_off : default?  # log : now semantic pos, now json pos?
    {
        "xxx"%optional = %int%event_larger_than_300
        "yyy" = 3
        "x" = {
            "z" = { %order_on %multiple_on %int%total_id }
            %string%can_empty_ut%optional = { %order_on %multiple_on 		
                %string%can_empty_ut%event_int_string%event_list_sum_larger_than_10
                    = {
                        %order_on %multiple_on %int%total_id 
                }
            }
        }
    }
    
 # Test Json File ?? ( "a.json" )
    {
        "xxx" : 333,
        "yyy" : 3,
        "x" : {	
            "z" : [1, 2, 3],
            "a,a" : { 
                "3" : [20], 
                "4" : [1, 2, 3,5], 
                "566" : [11]
            }, 
            "a,b" : { 
                "4" : [0] 
            }
        }
    }
