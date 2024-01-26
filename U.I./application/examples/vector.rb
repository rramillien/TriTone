#  frozen_string_literal: true

edition = "M257.7 752c2 0 4-0.2 6-0.5L431.9 722c2-0.4 3.9-1.3 5.3-2.8l423.9-423.9c3.9-3.9 3.9-10.2 0-14.1L694.9 114.9c-1.9-1.9-4.4-2.9-7.1-2.9s-5.2 1-7.1 2.9L256.8 538.8c-1.5 1.5-2.4 3.3-2.8 5.3l-29.5 168.2c-1.9 11.1 1.5 21.9 9.4 29.8 6.6 6.4 14.9 9.9 23.8 9.9z m67.4-174.4L687.8 215l73.3 73.3-362.7 362.6-88.9 15.7 15.6-89zM880 836H144c-17.7 0-32 14.3-32 32v36c0 4.4 3.6 8 8 8h784c4.4 0 8-3.6 8-8v-36c0-17.7-14.3-32-32-32z"

v = vector({ data: { path: { d: edition, id: :p1, stroke: :black, 'stroke-width' => 37, fill: :red } } })

wait 1 do
  v.data(circle: { cx: 1000, cy: 1000, r: 340, id: :p2, stroke: :green, 'stroke-width' => 35, fill: :yellow })
  wait 1 do
    v.color(:cyan) # colorise everything with the color method
    wait 1 do
      v.shadow({
                 id: :s4,
                 left: 20, top: 0, blur: 9,
                 option: :natural,
                 red: 0, green: 1, blue: 0, alpha: 1
               })
      v.component(p2: {fill: :blue,'stroke-width' => 166 })
      v.left(222)
    end
  end
end

