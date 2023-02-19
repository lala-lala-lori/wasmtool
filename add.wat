(module
    (func $add (param $A i32) (param $B i32) (result i32)
        get_local $A
        get_local $B
        i32.add
    )
    (export "add" (func $add))
)
