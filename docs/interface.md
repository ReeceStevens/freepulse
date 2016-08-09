# Interface Documentation

The interface model for FreePulse is as follows:

```
Screen (top level)
    |__ Screen Element
    |__ Screen Element
    |   |__ Screen Element
    |   |__ Screen Element
    |__ Screen Element (etc.)
```

## `Screen` 
`Screen` has three primary methods:
1. `add(const ScreenElement& e)` adds a `ScreenElement` object to the 
   `Screen`'s Vector of display elements.
2. `initalDraw()` renders the initial view for the screen by calling the `draw`
   method on each element
3. `update()` redraws any `ScreenElement` objects that have dynamically 
   changing data by calling the `update` method on each element

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
