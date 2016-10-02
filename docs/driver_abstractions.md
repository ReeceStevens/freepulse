# Device Driver Abstractions

## GPIO

The user-facing interface for GPIO pins is represented by the `Pin_Num` enum
values. This allows any function accepting a GPIO pin as an input to use the
`Pin_Num` type as the type of its argument. This enables the following syntax:

```
some_gpio_action(PA5);
some_other_gpio_action(PC7);
some_third_gpio_action(PB2);
```

When writing functions that use GPIO pins, the following call structure should
be in place:

```
// For digital/analog GPIO
void gpio_process(Pin_Num p) {
    configure_GPIO(p, <pull_up_down_setting>, <INPUT/OUTPUT/ANALOG>)
    // ... do GPIO stuff.
}
```

The logic of choosing the correct GPIO device (A, B, or C) and the correct pin
mapping is accomplished via the `Pin` class. *The user should never have to
access or interface with the `Pin` class; it is purely internal*.

GPIO pins are represented by the `Pin` class. `Pin`s have a mapping to the STM
standard peripheral drivers via the `GPIO_TypeDef* GPIOx` and `int
physical_pin` attributes.
