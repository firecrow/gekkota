/**
 * Simple utility function for placing a value in the middle of a transition
 *
 * for example, where in the delta between 2.0 and 5.0 is the value of 3.0
 *
 *     position_in_transition(3.0, 2.0, 5.0) is  0.33ish through the delta
 *
 */
double position_in_transition(double value, double start, double end);