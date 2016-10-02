# Interface Documentation

The interface model for FreePulse is as follows:

```
Screen (top level)
    |__ Screen Element
    |__ Screen Element
    |   |__ Screen Element
    |   |__ Screen Element
    |__ Button
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

4. `propogateTouch()`

    Checks each `Button` object to see if one has been touched via the 
    `Button::updateIfTapped()` method. If a button has been tapped, 
    it will call its `reducer` function before returning, propogating its
    change to the global state.

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

# Interface Lifecycle Hooks

The interface is updated in two distict stages:

1. Update Screen

   This redraws all dynamically updating interface components by calling the
   `update()` function. 

2. Propogate Touch Events

   If there is a touch event, propogates the event down the list of button
   elements and fires the `reducer()` method on the element that the event
   occured on.
