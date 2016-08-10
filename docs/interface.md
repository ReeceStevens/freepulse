# Interface Documentation

The interface model for FreePulse is as follows:

```
Screen (top level)
    |__ Screen Element
    |__ Screen Element
    |   |__ Screen Element
    |   |__ Screen Element
    |__ Button
        |__ -> Action
                |__ Type
                |__ Value
```

## `Screen` 
The primary methods of `Screen` are:

1. `add(const ScreenElement& e)` 

    Adds a `ScreenElement` object to the `Screen`'s Vector of display elements.

2. `initalDraw()` 

    Renders the initial view for the screen by calling the `draw` method on
    each element.

3. `update()` 

    Redraws any `ScreenElement` objects that have dynamically changing data 
    by calling the `update` method on each element.

4. `listenForTouch()`

    Checks each `Button` object to see if one has been touched via the 
    `Button::isTapped()` method. If a button has been tapped, returns an action
    object.

## `ScreenElement`
`ScreenElement` is the generic class that describes any element that can be 
displayed on the screen. All `ScreenElement` objects have a complete set of
coordinates describing their location on the screen in relative and absolute
coordinates. In addition, they have two methods:

1. `virtual void draw(void)` performs the initial render for the object

2. `virtual void update(void)` performs any update tasks that may be required

By default, both `draw` and `update` are simply empty functions. Any derived
class of `ScreenElement` defines `draw` and `update` as necessary.

### Derived Classes of `ScreenElement`
- Button
- TextBox
- LargeNumberView
- SignalTrace

## `Action`
An `Action` object has two attributes: `type` and `value`. The different types of 
actions include:

- ScreenChange

    Changes the screen currently being displayed to the one indicated by the action
    value.

- IncrementCounter

    Increments the counter pointed to by the action value.

- DecrementCounter

    Decrements the counter pointed to by the action value.

This list may grow and evolve during development.

All `Action` objects are reduced to a global state change via the reducing function in `main.cpp`.
