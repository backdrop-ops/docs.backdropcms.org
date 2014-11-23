/**
 * @file
 * Class inheritance loop.
 */

class A extends B {
}

Class B extends C {
}

Class C extends A {
}
