require "AssessmentBase.rb"

module Riscv
  include AssessmentBase

  def assessmentInitialize(course)
    super("riscv",course)
    @problems = []
  end

end
