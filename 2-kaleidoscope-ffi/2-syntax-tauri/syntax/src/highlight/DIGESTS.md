## Subresource Integrity

If you are loading Highlight.js via CDN you may wish to use [Subresource Integrity](https://developer.mozilla.org/en-US/docs/Web/Security/Subresource_Integrity) to guarantee that you are using a legimitate build of the library.

To do this you simply need to add the `integrity` attribute for each JavaScript file you download via CDN. These digests are used by the browser to confirm the files downloaded have not been modified.

```html
<script
  src="//cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js"
  integrity="sha384-9mu2JKpUImscOMmwjm1y6MA2YsW3amSoFNYwKeUHxaXYKQ1naywWmamEGMdviEen"></script>
<!-- including any other grammars you might need to load -->
<script
  src="//cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/languages/go.min.js"
  integrity="sha384-WmGkHEmwSI19EhTfO1nrSk3RziUQKRWg3vO0Ur3VYZjWvJRdRnX4/scQg+S2w1fI"></script>
```

The full list of digests for every file can be found below.

### Digests

```
sha384-3UnsR8EJG6uDdxeyWqOyV/M9UI5m8cCdd4rxGUokKsQeslxONZ1n7bef9Li90X+d /es/languages/ebnf.js
sha384-jQuTDS10jALGEitp2l3m/2rTJZKkqrZAYnpPu/l1hGq+/7OdVpCLIZFBsqeyLD1k /es/languages/ebnf.min.js
sha384-nqo0+QkWUpRKcCCzbVTXBzQ2t1ng8RTjsclnLwLysnSpLJvGfko8hq30rU9HJVq4 /languages/ebnf.js
sha384-LT6ea03Ihj20gYuU/bfouACcd8rNgZAohWItMlIwiS8CUEIBt5BEeVBZj/Xzngp/ /languages/ebnf.min.js
sha384-SI6B+oKA2ldTnH0uMnTbN4nAezjR2+UrAYy4GJpQ6OGwGqxBqR3AFgskl4ic9C0y /highlight.js
sha384-Qu7+6WJ7tqNI9q3Qh5JP8WVla39DxjkPt/urauyQMMMe9B+Se6YiPS9s+7pxDyA2 /highlight.min.js
```

