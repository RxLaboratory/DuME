## LUT Implementation

à reprendre:

il faut à chaque étape garder le trc/primaries courant pour le donner aux algos de conversions comme input.

- if LUT added before color management :
    - check if trc and primaries on input need to be changed DONE
        - use LUT output metadata for trc and primaries on color interpretation for inputs  
        DONE mais pas bon : le zscale de conversion a besoin des bonnes métadata
        - convert trc and primaries before applying lut on output WIP line 442 in FFmpegRenderer
        - apply lut
- if LUT added after color management :
    - check if trc and primaries on output need to be changed
        - convert output trc and primaries if needed
        - apply lut

## Windows / Mac (Ae)

- [ ] Detect ae errors

## Filters to add

- denoise (bm3d)
- interlace (add to the existing deinterlace block)
- premultiply (add to the existing unmultiply block)
