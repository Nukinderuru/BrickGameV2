# Snake FSM

States:

- `Start`
- `Spawn`
- `Moving`
- `GameOver`
- `Win`

Transitions:

- `Start -> Spawn`: start button
- `Spawn -> Moving`: initial snake and apple are placed
- `Moving -> Moving`: next movement tick succeeds
- `Moving -> GameOver`: wall collision
- `Moving -> GameOver`: self collision
- `Moving -> GameOver`: terminate action
- `Moving -> Win`: snake length reaches 200
- `GameOver -> Spawn`: start button begins a new game
- `Win -> Spawn`: start button begins a new game

Pause is implemented as a flag that freezes `Moving` updates instead of a
separate FSM state.
