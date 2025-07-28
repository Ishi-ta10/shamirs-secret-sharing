"use client"

import { useState } from "react"
import { Button } from "@/components/ui/button"
import { Textarea } from "@/components/ui/textarea"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Alert, AlertDescription } from "@/components/ui/alert"
import { Badge } from "@/components/ui/badge"

export default function ShamirsSecretSharing() {
  const [jsonInput, setJsonInput] = useState("")
  const [result, setResult] = useState<{
    secret: string
    wrongShares: number[]
    totalCombinations: number
    validCombinations: number
  } | null>(null)
  const [error, setError] = useState("")
  const [loading, setLoading] = useState(false)

  // Function to evaluate mathematical expressions
  const evaluateExpression = (expr: string): bigint => {
    // Handle different function formats
    if (expr.includes("sum(")) {
      const match = expr.match(/sum$$(\d+),\s*(\d+)$$/)
      if (match) {
        return BigInt(match[1]) + BigInt(match[2])
      }
    }

    if (expr.includes("multiply(")) {
      const match = expr.match(/multiply$$(\d+),\s*(\d+)$$/)
      if (match) {
        return BigInt(match[1]) * BigInt(match[2])
      }
    }

    if (expr.includes("divide(")) {
      const match = expr.match(/divide$$(\d+),\s*(\d+)$$/)
      if (match) {
        return BigInt(match[1]) / BigInt(match[2])
      }
    }

    if (expr.includes("lcm(")) {
      const match = expr.match(/lcm$$(\d+),\s*(\d+)$$/)
      if (match) {
        const a = BigInt(match[1])
        const b = BigInt(match[2])
        return (a * b) / gcd(a, b)
      }
    }

    if (expr.includes("hcf(") || expr.includes("gcd(")) {
      const match = expr.match(/(?:hcf|gcd)$$(\d+),\s*(\d+)$$/)
      if (match) {
        return gcd(BigInt(match[1]), BigInt(match[2]))
      }
    }

    // If it's just a number
    if (/^\d+$/.test(expr.trim())) {
      return BigInt(expr.trim())
    }

    throw new Error(`Unknown expression format: ${expr}`)
  }

  // Greatest Common Divisor using Euclidean algorithm
  const gcd = (a: bigint, b: bigint): bigint => {
    while (b !== 0n) {
      ;[a, b] = [b, a % b]
    }
    return a < 0n ? -a : a
  }

  // Modular inverse using Extended Euclidean Algorithm
  const modInverse = (a: bigint, m: bigint): bigint => {
    if (gcd(a, m) !== 1n) {
      throw new Error("Modular inverse does not exist")
    }

    let [old_r, r] = [a, m]
    let [old_s, s] = [1n, 0n]

    while (r !== 0n) {
      const quotient = old_r / r
      ;[old_r, r] = [r, old_r - quotient * r]
      ;[old_s, s] = [s, old_s - quotient * s]
    }

    return old_s < 0n ? old_s + m : old_s
  }

  // Lagrange interpolation to find the constant term (secret)
  const lagrangeInterpolation = (points: Array<{ x: bigint; y: bigint }>): bigint => {
    const PRIME = 2n ** 127n - 1n // Large prime for modular arithmetic

    let result = 0n
    const n = points.length

    for (let i = 0; i < n; i++) {
      let term = points[i].y
      let numerator = 1n
      let denominator = 1n

      for (let j = 0; j < n; j++) {
        if (i !== j) {
          numerator = (numerator * (0n - points[j].x)) % PRIME
          denominator = (denominator * (points[i].x - points[j].x)) % PRIME
        }
      }

      // Handle negative numerator
      if (numerator < 0n) {
        numerator = ((numerator % PRIME) + PRIME) % PRIME
      }

      // Handle negative denominator
      if (denominator < 0n) {
        denominator = ((denominator % PRIME) + PRIME) % PRIME
      }

      const inv = modInverse(denominator, PRIME)
      term = (((term * numerator) % PRIME) * inv) % PRIME
      result = (result + term) % PRIME
    }

    return result < 0n ? result + PRIME : result
  }

  // Generate all combinations of k elements from n elements
  const getCombinations = (arr: any[], k: number): any[][] => {
    if (k === 1) return arr.map((el) => [el])
    if (k === arr.length) return [arr]

    const combinations: any[][] = []

    for (let i = 0; i <= arr.length - k; i++) {
      const head = arr[i]
      const tailCombinations = getCombinations(arr.slice(i + 1), k - 1)
      tailCombinations.forEach((tail) => combinations.push([head, ...tail]))
    }

    return combinations
  }

  const solveShamirsSecret = () => {
    setLoading(true)
    setError("")
    setResult(null)

    try {
      const data = JSON.parse(jsonInput)
      const n = data.n
      const k = data.k
      const shares = data.shares || data.keys

      if (!shares || shares.length !== n) {
        throw new Error("Invalid input: shares length does not match n")
      }

      // Evaluate all share values
      const evaluatedShares = shares.map((share: any, index: number) => ({
        id: share.id || share.key || index + 1,
        value: evaluateExpression(share.value),
      }))

      // Generate all combinations of k shares from n shares
      const combinations = getCombinations(evaluatedShares, k)
      const secretCounts = new Map<string, number>()
      const secretToCombination = new Map<string, any[]>()

      // Try each combination
      combinations.forEach((combination) => {
        try {
          const points = combination.map((share) => ({
            x: BigInt(share.id),
            y: share.value,
          }))

          const secret = lagrangeInterpolation(points)
          const secretStr = secret.toString()

          secretCounts.set(secretStr, (secretCounts.get(secretStr) || 0) + 1)
          if (!secretToCombination.has(secretStr)) {
            secretToCombination.set(secretStr, combination)
          }
        } catch (err) {
          // Skip invalid combinations
        }
      })

      // Find the most common secret (correct one)
      let maxCount = 0
      let correctSecret = ""

      for (const [secret, count] of secretCounts.entries()) {
        if (count > maxCount) {
          maxCount = count
          correctSecret = secret
        }
      }

      if (!correctSecret) {
        throw new Error("Could not determine the correct secret")
      }

      // Find wrong shares by identifying which shares never appear in valid combinations
      const validShareIds = new Set<number>()
      const correctCombination = secretToCombination.get(correctSecret)

      // Find all combinations that produce the correct secret
      combinations.forEach((combination) => {
        try {
          const points = combination.map((share) => ({
            x: BigInt(share.id),
            y: share.value,
          }))

          const secret = lagrangeInterpolation(points)
          if (secret.toString() === correctSecret) {
            combination.forEach((share) => validShareIds.add(share.id))
          }
        } catch (err) {
          // Skip
        }
      })

      const wrongShares = evaluatedShares.filter((share) => !validShareIds.has(share.id)).map((share) => share.id)

      setResult({
        secret: correctSecret,
        wrongShares,
        totalCombinations: combinations.length,
        validCombinations: maxCount,
      })
    } catch (err) {
      setError(err instanceof Error ? err.message : "An error occurred")
    } finally {
      setLoading(false)
    }
  }

  const exampleJson = `{
  "n": 5,
  "k": 3,
  "shares": [
    {"id": 1, "value": "sum(123456789012345, 987654321098765)"},
    {"id": 2, "value": "multiply(12345, 67890)"},
    {"id": 3, "value": "lcm(48, 18)"},
    {"id": 4, "value": "hcf(48, 18)"},
    {"id": 5, "value": "divide(1000000, 7)"}
  ]
}`

  return (
    <div className="container mx-auto p-6 max-w-4xl">
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">🔐 Shamir's Secret Sharing Algorithm</CardTitle>
          <CardDescription>
            Implement cryptographic secret sharing with error detection for nuclear-level security
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-6">
          <div>
            <label className="block text-sm font-medium mb-2">JSON Input (paste your input here):</label>
            <Textarea
              value={jsonInput}
              onChange={(e) => setJsonInput(e.target.value)}
              placeholder={exampleJson}
              className="min-h-[200px] font-mono text-sm"
            />
            <Button onClick={() => setJsonInput(exampleJson)} variant="outline" size="sm" className="mt-2">
              Load Example
            </Button>
          </div>

          <Button onClick={solveShamirsSecret} disabled={loading || !jsonInput.trim()} className="w-full">
            {loading ? "Computing Secret..." : "Solve Secret Sharing"}
          </Button>

          {error && (
            <Alert variant="destructive">
              <AlertDescription>{error}</AlertDescription>
            </Alert>
          )}

          {result && (
            <div className="space-y-4">
              <Alert>
                <AlertDescription>
                  <div className="space-y-2">
                    <div className="font-semibold">🎯 Secret Found:</div>
                    <div className="font-mono text-lg bg-muted p-2 rounded break-all">{result.secret}</div>
                  </div>
                </AlertDescription>
              </Alert>

              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                <Card>
                  <CardHeader className="pb-2">
                    <CardTitle className="text-sm">Statistics</CardTitle>
                  </CardHeader>
                  <CardContent className="space-y-2">
                    <div>
                      Total Combinations: <Badge variant="outline">{result.totalCombinations}</Badge>
                    </div>
                    <div>
                      Valid Combinations: <Badge variant="outline">{result.validCombinations}</Badge>
                    </div>
                  </CardContent>
                </Card>

                <Card>
                  <CardHeader className="pb-2">
                    <CardTitle className="text-sm">Wrong Shares Detected</CardTitle>
                  </CardHeader>
                  <CardContent>
                    {result.wrongShares.length > 0 ? (
                      <div className="flex flex-wrap gap-1">
                        {result.wrongShares.map((id) => (
                          <Badge key={id} variant="destructive">
                            Share {id}
                          </Badge>
                        ))}
                      </div>
                    ) : (
                      <Badge variant="secondary">No wrong shares detected</Badge>
                    )}
                  </CardContent>
                </Card>
              </div>
            </div>
          )}

          <Card className="bg-muted/50">
            <CardHeader className="pb-2">
              <CardTitle className="text-sm">Algorithm Overview</CardTitle>
            </CardHeader>
            <CardContent className="text-sm space-y-2">
              <div>• Evaluates mathematical expressions in share values</div>
              <div>• Uses Lagrange interpolation to reconstruct polynomials</div>
              <div>• Tries all C(n,k) combinations to find the correct secret</div>
              <div>• Identifies wrong shares through consensus mechanism</div>
              <div>• Handles large numbers (20-40 digits) using BigInt</div>
            </CardContent>
          </Card>
        </CardContent>
      </Card>
    </div>
  )
}
